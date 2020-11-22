
#include "CPUMesh.hpp"

void CPUMesh::loadFBX(Path path) {

	//check if file is valid
	bool error = false;

	if (std::filesystem::exists(path.parent_path())) {

		logEvent(String("Loading mesh: (").append(name.data).append(") from (").append(path.string()).append(")"));

		//create the mesh
		name = path.filename().replace_extension().string();
		primitiveMode = Triangles;
		Assimp::Importer Importer;
		const aiScene* scene = Importer.ReadFile(path.string(),
												 aiProcess_CalcTangentSpace |
												 aiProcess_GenSmoothNormals |
												 aiProcess_JoinIdenticalVertices |
												 aiProcess_SortByPType |
												 aiProcess_Triangulate
		);

		if (scene) {
			if (scene->HasMeshes()) {
				if (scene->mMeshes[0]->HasPositions()) {
					for (unsigned int i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
						glm::vec3 vertex;
						vertex.x = scene->mMeshes[0]->mVertices[i].x;
						vertex.y = scene->mMeshes[0]->mVertices[i].y;
						vertex.z = scene->mMeshes[0]->mVertices[i].z;
						vertices.push_back(vertex);

						glm::vec2 texCoord;
						texCoord.x = scene->mMeshes[0]->mTextureCoords[0][i].x;
						texCoord.y = scene->mMeshes[0]->mTextureCoords[0][i].y;
						uvs.push_back(texCoord);
						
						glm::vec3 normal;
						normal.x = scene->mMeshes[0]->mNormals[i].x;
						normal.y = scene->mMeshes[0]->mNormals[i].y;
						normal.z = scene->mMeshes[0]->mNormals[i].z;
						normals.push_back(normal);
					}
					if (scene->mMeshes[0]->HasFaces()) {
						for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
							for (unsigned int j = 0; j < scene->mMeshes[0]->mFaces->mNumIndices; j++) { //should always be 3 (0 -> 1 -> 2 <<)
								unsigned int index = scene->mMeshes[0]->mFaces[i].mIndices[j];
								indices.push_back(index);
							}
						}
					}
					else error = true;
				}
				else error = true;
			}
			else error = true;
		}
		else error = true;
	}
	else {
		error = true;
		logError(path.parent_path().string().append(" does not exist!"));
	}

	if (error) {
		logDebug(String("The model (").append(name.data).append(") could not be loaded! (").append(path.string()).append(")"));
		readyForUpload = false;
		return;
	}
	else {
		readyForUpload = true;

		saveMeshFile();
	}
}

void CPUMesh::generate2DTerrainMesh(Terrain& terrain)
{
	readyForUpload = false;
	name = "terrain";
	primitiveMode = Triangles;

	vertices.clear();
	indices.clear();
	normals.clear();
	uvs.clear();

	//for (int y = 0; y < TERRAIN_DETAIL+1; y++) {
	//	for (int x = 0; x < TERRAIN_DETAIL+1; x++) {
	//		Vec3 tmp = Vec3(Float(x) / Float(TERRAIN_DETAIL+1), Float(y) / Float(TERRAIN_DETAIL+1), terrain.heightmap[x][y]);

	//		mesh.vertices.push_back(tmp);
	//		mesh.normals.push_back(Vec3(0, 0, 1));
	//		mesh.uvs.push_back(Vec2(tmp.x, tmp.y));

	//		if ((x < TERRAIN_DETAIL) & (y < TERRAIN_DETAIL)) {
	//			//first triangle
	//			mesh.indices.push_back(y * TERRAIN_DETAIL + x);
	//			mesh.indices.push_back(y * TERRAIN_DETAIL + x + 1);
	//			mesh.indices.push_back((y + 1) * TERRAIN_DETAIL + x);
	//			
	//			//second triangle
	//			mesh.indices.push_back((y + 1) * TERRAIN_DETAIL + x);
	//			mesh.indices.push_back(y * TERRAIN_DETAIL + x + 1);
	//			mesh.indices.push_back((y + 1) * TERRAIN_DETAIL + x + 1);
	//		}
	//	}
	//}

	vertices.push_back(Vec3(0, 0, 0));
	vertices.push_back(Vec3(0, 1, 0));
	vertices.push_back(Vec3(1, 0, 0));
	vertices.push_back(Vec3(1, 1, 0));
	normals.push_back(Vec3(0, 0, 1));
	normals.push_back(Vec3(0, 0, 1));
	normals.push_back(Vec3(0, 0, 1));
	normals.push_back(Vec3(0, 0, 1));
	uvs.push_back(Vec2(0, 0));
	uvs.push_back(Vec2(0, 1));
	uvs.push_back(Vec2(1, 0));
	uvs.push_back(Vec2(1, 1));
	indices.push_back(0);
	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);


	readyForUpload = true;
}

void CPUMesh::autoLoadFromFile(Name name)
{
	Path meshPath = String("Data/meshes/").append(name.data).append(".mesh");
	Path fbxPath = String("Data/objects/").append(name.data).append(".fbx");

	Bool f1, f2;
	f1 = std::filesystem::exists(meshPath);
	f2 = std::filesystem::exists(fbxPath);

	if (f1) {
		if (f2) {
			auto t1 = std::filesystem::last_write_time(meshPath);
			auto t2 = std::filesystem::last_write_time(fbxPath);
			if (t1 > t2)
				loadMeshFile(meshPath);
			else
				loadFBX(fbxPath);
		}
		else
			loadMeshFile(meshPath);
	}
	else {
		if (f2)
			loadFBX(fbxPath);
		else
			logDebug(String("Can not find mesh with name: (").append(name.data).append(")"));
	}
}

void CPUMesh::saveMeshFile()
{
	Path path = String("Data/meshes/").append(name.data).append(".mesh");

	logEvent(String("Saving mesh: (").append(name.data).append(") from(").append(path.string()).append(")"));

	MeshHeaderV2 header;
	header.meshName = name;
	header.primitiveMode = Triangles;
	header.vertexCount = vertices.size();
	header.uvCount = uvs.size();
	header.normalCount = normals.size();
	header.indexCount = indices.size();



	while (!std::filesystem::exists(path.parent_path())) {
		logDebug(String("The directory (").append(path.parent_path().string()).append(") does not exist and will be created!"));
		std::filesystem::create_directory(path.parent_path());
	}

	std::ofstream file(path, std::ios::trunc | std::ios::binary | std::ios::out);
	if (file.is_open()) {
		file.write((char*)&header, sizeof(MeshHeaderV2));
		file.write((char*)&vertices[0], vertices.size() * sizeof(Vec3));
		file.write((char*)&uvs[0], uvs.size() * sizeof(Vec2));
		file.write((char*)&normals[0], normals.size() * sizeof(Vec3));
		file.write((char*)&indices[0], indices.size() * sizeof(Index));
		file.close();
	}
	else {
		logDebug("The mesh binary file could not be opened!");
	}
}

void CPUMesh::loadMeshFile(Path path) {
	bool error = false;

	std::ifstream ifstream(path, std::ios::binary | std::ios::in);

	logEvent(String("Loading mesh file from(").append(path.string()).append(")"));

	if (ifstream.is_open()) {
		MeshHeaderV2 header;
		ifstream.read((char*)&header, sizeof(MeshHeaderV2));

		if (header.version == 2) {
			name = header.meshName;
			primitiveMode = header.primitiveMode;

			//load vertices
			vertices.resize(header.vertexCount);
			ifstream.read((char*)&vertices[0], header.vertexCount * sizeof(Vec3));

			//load texture coordinates
			uvs.resize(header.uvCount);
			ifstream.read((char*)&uvs[0], header.uvCount * sizeof(Vec2));

			//load normals
			normals.resize(header.normalCount);
			ifstream.read((char*)&normals[0], header.normalCount * sizeof(Vec3));

			//load indices
			indices.resize(header.indexCount);
			ifstream.read((char*)&indices[0], header.indexCount * sizeof(Index));

			auto pos1 = ifstream.tellg();
			ifstream.eof();
			auto pos2 = ifstream.tellg();
			if (pos1 == pos2) {
				logEvent(String("The mesh (").append(name.data).append(") was loaded successfully!"));
			}
			else {
				logDebug(String("Error: The mesh (").append(name.data).append(") was loaded, but did not reach the end of the file!"));
			}
		}
		else {
			logError("Mesh version not supported!");
			error = true;
		}

		ifstream.close();
	}
	else error = true;



	if (error) {
		logEvent(String("The model (").append(path.filename().replace_extension().string()).append(") could not be loaded! (").append(path.string()).append(")"));
		readyForUpload = false;
		return;
	}
	else {
		readyForUpload = true;
	}

};
