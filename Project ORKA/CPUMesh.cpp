
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"
#include "Renderer.hpp"

CPUMesh::CPUMesh()
{
}
CPUMesh::~CPUMesh() {}
CPUMesh::CPUMesh(Graph& graph) {
	drawMode = MeshDrawMode::dynamicMode;
	name = "graph";
	primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < graph.points.size(); index++) {
		Vec2 pointPos = Vec2( Float(index) / Float(graph.points.size()), graph.points[index]);

		vertices.push_back(Vec3(pointPos, 0));
		uvs.push_back(pointPos);
		normals.push_back(Vec3(0, 0, 1));

		indices.push_back(index);
	}

	checkIntegrity();
}
CPUMesh::CPUMesh(Name name, MeshDrawMode drawMode, PrimitiveMode primitiveMode) {
	this->name = name;
	this->drawMode = drawMode;
	this->primitiveMode = primitiveMode;
	indices.clear();
	vertices.clear();
	normals.clear();
	uvs.clear();
	readyForUpload = false;
}
CPUMesh::CPUMesh(Name name)
{
	load(name);
}

void CPUMesh::saveMeshFile()
{
	MeshHeader header;
	header.meshName = name;
	header.primitiveMode = PrimitiveMode::Triangles;
	header.vertexCount = vertices.size();
	header.uvCount = uvs.size();
	header.normalCount = normals.size();
	header.indexCount = indices.size();

	OutFile mesh(String("Data/meshes/").append(name.data).append(".mesh"));
	mesh.write((Char*)&header, sizeof(MeshHeader));
	mesh.write((Char*)&vertices[0], vertices.size() * sizeof(Vec3));
	mesh.write((Char*)&uvs[0], uvs.size() * sizeof(Vec2));
	mesh.write((Char*)&normals[0], normals.size() * sizeof(Vec3));
	mesh.write((Char*)&indices[0], indices.size() * sizeof(Index));
}
void CPUMesh::load(Name name)
{
	Path meshPath = String("Data/meshes/").append(name.data).append(".mesh");
	Path fbxPath = String("Data/objects/").append(name.data).append(".fbx");

	Bool f1 = doesPathExist(meshPath);
	Bool f2 = doesPathExist(fbxPath);

	if (f1) {
		if (f2) {
			auto t1 = std::filesystem::last_write_time(meshPath);
			auto t2 = std::filesystem::last_write_time(fbxPath);
			if (t1 > t2) loadMeshFile(meshPath);
			else loadFBX(fbxPath);
		}
		else loadMeshFile(meshPath);
	}
	else {
		if (f2)loadFBX(fbxPath);
		else logDebug(String("Can not find mesh with name: (").append(name.data).append(")"));
	}
}
void CPUMesh::removeDoubles() {
	for (Int i = 0; i < vertices.size(); i++) {

		Vec3 current = vertices[i];
		Index currentID = i;

		for (Int j = 0; j < vertices.size(); j++) {
			if (vertices[j] == current) {

				for (Int k = 0; k < indices.size(); k++) {
					if (indices[k] == j)  indices[k] = currentID;
				}
			}
		}
	}
}
void CPUMesh::checkIntegrity() {
	if (uvs.size() == vertices.size() && normals.size() == vertices.size()) readyForUpload = true;
	else readyForUpload = false;
}
void CPUMesh::loadFBX(Path path) {

	//check if file is valid
	String errorMessage = "";

	if (std::filesystem::exists(path.parent_path())) {

		logEvent(String("Loading mesh: (").append(name.data).append(") from (").append(path.string()).append(")"));

		//create the mesh
		name = path.filename().replace_extension().string();
		primitiveMode = PrimitiveMode::Triangles;
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
					for (UInt i = 0; i < scene->mMeshes[0]->mNumVertices; i++) {
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
					else errorMessage = "Mesh does not have faces!";
				}
				else errorMessage = "Mesh does not have positions stored!";
			}
			else errorMessage = "No meshes in fbx scene!";
		}
		else errorMessage = "No Scene in fbx file!";
	}
	else {
		errorMessage = path.parent_path().string().append(" does not exist!");
	}

	if (errorMessage.size()) {

		logDebug(String("The model (").append(name.data).append(") could not be loaded! (").append(path.string()).append(")").append(" Error: ").append(errorMessage));
		readyForUpload = false;
		return;
	}
	else {
		checkIntegrity();

		saveMeshFile();
	}
}
void CPUMesh::merge(CPUMesh source) {
	for (Int i = 0; i < source.vertices.size(); i++) {
		vertices.push_back(source.vertices[i]);
		uvs.push_back(source.uvs[i]);
		normals.push_back(source.normals[i]);
	}

	Int indexOffset = vertices.size();

	for (Int i = 0; i < source.indices.size(); i++) {
		indices.push_back(source.indices[i] + indexOffset);
	}
}
void CPUMesh::move(Vec3 moveVector) {
	for (Int i = 0; i < vertices.size(); i++) {
		vertices[i] += moveVector;
	}
}
void CPUMesh::loadMeshFile(Path path) {
	bool error = false;

	InFile file(path);

	logEvent(String("Loading mesh file from(").append(path.string()).append(")"));

	if (file.isOpen) {
		MeshHeader header;
		file.read((char*)&header, sizeof(MeshHeader));

		if (header.version == 1) {
			name = header.meshName;
			primitiveMode = header.primitiveMode;

			vertices.resize(header.vertexCount);
			uvs.resize(header.uvCount);
			normals.resize(header.normalCount);
			indices.resize(header.indexCount);

			file.read((char*)&vertices[0], header.vertexCount * sizeof(Vec3));
			file.read((char*)&uvs[0], header.uvCount * sizeof(Vec2));
			file.read((char*)&normals[0], header.normalCount * sizeof(Vec3));
			file.read((char*)&indices[0], header.indexCount * sizeof(Index));
		}
		else {
			logError("Mesh version not supported!");
			error = true;
		}
	}
	else error = true;

	if (error) {
		logEvent(String("The model (").append(path.filename().replace_extension().string()).append(") could not be loaded! (").append(path.string()).append(")"));
		readyForUpload = false;
		return;
	}
	else {
		checkIntegrity();
	}
};
void CPUMesh::calculateSmoothNormals()
{
	switch (primitiveMode) {
	case PrimitiveMode::Triangles:
		normals.resize(vertices.size());
		for (Vec3& normal : normals) {
			normal = Vec3(0);
		}

		for (UInt f = 0; f < (indices.size() / 3); f++) {
			Index indexA = indices[f * 3];
			Index indexB = indices[f * 3 + 1];
			Index indexC = indices[f * 3 + 2];

			//get vertex position for each face
			Vec3 vertexA = vertices[indexA];
			Vec3 vertexB = vertices[indexB];
			Vec3 vertexC = vertices[indexC];

			//get face normal
			Vec3 faceNormal = glm::normalize(glm::cross(vertexB - vertexA, vertexC - vertexA));

			//add face normal to every vertex
			normals[indexA] += faceNormal;
			normals[indexB] += faceNormal;
			normals[indexC] += faceNormal;
		}
		for (Vec3& normal : normals) {
			normal = glm::normalize(normal);
		}
		break;
	case PrimitiveMode::TriangleStrip:
		normals.resize(vertices.size());

		for (Vec3& normal : normals) {
			normal = Vec3(0);
		}

		for (UInt f = 2; f < indices.size(); f++) {

			Index a, b, c;
			if (isEven(f)) {
				a = indices[f - 2];
				b = indices[f - 1];
				c = indices[f - 0];
			}
			else {
				a = indices[f - 1];
				b = indices[f - 2];
				c = indices[f - 0];
			}

			//get vertex position for each face
			Vec3 vertexA = vertices[a];
			Vec3 vertexB = vertices[b];
			Vec3 vertexC = vertices[c];
			
			if(vertexA == vertexB || vertexB == vertexC || vertexA == vertexC)continue;

			//get face normal
			Vec3 faceNormal = glm::normalize(glm::cross(vertexB - vertexA, vertexC - vertexA));

			//add face normal to every vertex
			normals[a] += faceNormal;
			normals[b] += faceNormal;
			normals[c] += faceNormal;
		}
		for (Vec3& normal : normals) {
			normal = glm::normalize(normal);
		}
		break;
	default:
		logError("Cant compute normals for this mesh! Primitive type not supported!");
		break;
	}
}
void CPUMesh::render(Renderer & renderer) {
	GPUMesh mesh;
	mesh.upload(*this);
	mesh.render(renderer.uniforms());
	mesh.unload();
}