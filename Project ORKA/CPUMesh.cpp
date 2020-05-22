
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

	MeshHeaderV1 header;
	header.meshName = name;
	header.primitiveMode = Triangles;
	header.vertexCount = vertices.size();
	header.uvCount = uvs.size();
	header.indexCount = indices.size();



	while (!std::filesystem::exists(path.parent_path())) {
		logDebug(String("The directory (").append(path.parent_path().string()).append(") does not exist and will be created!"));
		std::filesystem::create_directory(path.parent_path());
	}

	std::ofstream file(path, std::ios::trunc | std::ios::binary | std::ios::out);
	if (file.is_open()) {
		file.write((char*)&header, sizeof(MeshHeaderV1));
		file.write((char*)&vertices[0], vertices.size() * sizeof(Vec3));
		file.write((char*)&uvs[0], uvs.size() * sizeof(Vec2));
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
		MeshHeaderV1 header;
		ifstream.read((char*)&header, sizeof(MeshHeaderV1));

		if (header.version == 1) {
			name = header.meshName;
			primitiveMode = header.primitiveMode;

			//load vertices
			vertices.resize(header.vertexCount);
			ifstream.read((char*)&vertices[0], header.vertexCount * sizeof(Vec3));

			//load texture coordinates
			uvs.resize(header.uvCount);
			ifstream.read((char*)&uvs[0], header.uvCount * sizeof(Vec2));

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
		else error = true;

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
