
#include "CPUMesh.hpp"

void CPUMesh::load(MeshFileInfo & info) {

#ifdef DEBUG
	std::cout << "Loading mesh: " << info.name << " from " << info.path;
#endif

	//create the mesh
	name = info.name;
	primitiveMode = info.primitiveMode;
	Assimp::Importer Importer;
	const aiScene* scene = Importer.ReadFile(info.path,
											 aiProcess_CalcTangentSpace |
											 aiProcess_GenSmoothNormals |
											 aiProcess_JoinIdenticalVertices |
											 aiProcess_SortByPType |
											 aiProcess_Triangulate
	);

	//check if file is valid
	bool error = false;
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
	if (error) std::cout << "The " << info.name << " model could not be loaded. (" << info.path << ")" << "\n";

	readyForUpload = true;
}

void CPUMesh::load(String name, PrimitiveMode mode, Path path) {

#ifdef DEBUG
	std::cout << "Loading mesh: " << info.name << " from " << info.path;
#endif

	//create the mesh
	this->name = name;
	primitiveMode = mode;
	Assimp::Importer Importer;
	const aiScene* scene = Importer.ReadFile(path.string(),
											 aiProcess_CalcTangentSpace |
											 aiProcess_GenSmoothNormals |
											 aiProcess_JoinIdenticalVertices |
											 aiProcess_SortByPType |
											 aiProcess_Triangulate
	);

	//check if file is valid
	bool error = false;
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
	if (error) {
		std::cout << "The " << name << " model could not be loaded. (" << path.string() << ")" << "\n";
		readyForUpload = false;
		return;
	}
	else {
		readyForUpload = true;
	}
}