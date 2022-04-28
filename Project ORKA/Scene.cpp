
#include "Scene.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void Scene::loadFBX(Path path) {
	//check if file is valid
	String errorMessage = "";

	if (std::filesystem::exists(path.parent_path())) {

		logEvent(String("Loading mesh: (").append(path.stem().string()).append(") from (").append(path.string()).append(")"));

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(path.string(),
			aiProcess_GenUVCoords |
			aiProcess_Triangulate |
			//aiProcess_SortByPType |
			//aiProcess_FindInvalidData |
			//aiProcess_FindDegenerates |
			//aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			//aiProcess_ImproveCacheLocality |
			//aiProcess_ValidateDataStructure |
			//aiProcess_JoinIdenticalVertices |
			aiProcess_RemoveRedundantMaterials
		);

		errorMessage = importer.GetErrorString();
		if (errorMessage == "") {

			if (assimpScene->HasMeshes()) {

				for (Index objectID = 0; objectID < assimpScene->mRootNode->mNumChildren; objectID++) {

					assimpScene->mRootNode->mChildren[objectID]->mName;

					if (assimpScene->mRootNode->mChildren[objectID]->mNumMeshes > 0) {

						//get all meshes of object
						UInt numMeshes = assimpScene->mRootNode->mChildren[objectID]->mNumMeshes;

						CPUMesh mesh;
						mesh.name = String(assimpScene->mRootNode->mChildren[objectID]->mName.C_Str());

						UInt lastIndex = 0;

						for (UInt objectsMeshID = 0; objectsMeshID < numMeshes; objectsMeshID++) {
							Index meshID = assimpScene->mRootNode->mChildren[objectID]->mMeshes[objectsMeshID];

							if (assimpScene->mMeshes[meshID]->HasPositions()) {
								for (UInt i = 0; i < assimpScene->mMeshes[meshID]->mNumVertices; i++) {
									mesh.primitiveMode = PrimitiveMode::Triangles;

									Vec3 vertex;
									vertex.x = assimpScene->mMeshes[meshID]->mVertices[i].x;
									vertex.y = assimpScene->mMeshes[meshID]->mVertices[i].y;
									vertex.z = assimpScene->mMeshes[meshID]->mVertices[i].z;
									mesh.vertices.push_back(vertex);

									Vec2 texCoord = Vec2(0);
									if (assimpScene->mMeshes[meshID]->mTextureCoords[0]) {
									texCoord.x = assimpScene->mMeshes[meshID]->mTextureCoords[0][i].x;
									texCoord.y = assimpScene->mMeshes[meshID]->mTextureCoords[0][i].y;
									}
									mesh.uvs.push_back(texCoord);

									Vec3 normal;
									normal.x = assimpScene->mMeshes[meshID]->mNormals[i].x;
									normal.y = assimpScene->mMeshes[meshID]->mNormals[i].y;
									normal.z = assimpScene->mMeshes[meshID]->mNormals[i].z;
									mesh.normals.push_back(normal);
								}

								if (assimpScene->mMeshes[meshID]->HasFaces()) {
									for (UInt i = 0; i < assimpScene->mMeshes[meshID]->mNumFaces; i++) {
										for (UInt j = 0; j < assimpScene->mMeshes[meshID]->mFaces->mNumIndices; j++) { //should always be 3 (0 -> 1 -> 2 <<)
											Index index = lastIndex + assimpScene->mMeshes[meshID]->mFaces[i].mIndices[j];
											mesh.indices.push_back(index);
										}
									}
								}
								else errorMessage = "Mesh does not have faces!";
							}
							else errorMessage = "Mesh does not have positions stored!";

							lastIndex += assimpScene->mMeshes[meshID]->mNumVertices;
						}
						if (errorMessage.size()) {
							logError(String("The model (").append(path.stem().string()).append(") could not be loaded! (").append(path.string()).append(")").append(" Error: ").append(errorMessage));
							mesh.readyForUpload = false;
							break;
						}
						else {
							mesh.checkIntegrity();
							mesh.saveMeshFile();
						}
						meshes.push_back(mesh);
					}
					else errorMessage = "Object in scene does not have meshes!";
				}
			}
			else errorMessage = "No meshes in fbx scene!";
		}
	}
	else {
		errorMessage = path.parent_path().string().append(" does not exist!");
	}

	if (errorMessage.size()) {
		logError(String("The model (").append(path.stem().string()).append(") could not be loaded! (").append(path.string()).append(")").append(" Error: ").append(errorMessage));
	}
}
