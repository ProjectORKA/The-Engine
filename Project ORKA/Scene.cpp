
#include "Scene.hpp"
#include "Transform.hpp"

using TReal = Float;

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void Scene::loadFBX(Path path, ResourceManager& resourceManager) {
	//check if file is valid
	String errorMessage = "";

	if (doesPathExist(getDirectory(path))) {

		logEvent(String("Loading mesh: (").append(path.stem().string()).append(") from (").append(path.string()).append(")"));

		Assimp::Importer importer;
		const aiScene* assimpScene = importer.ReadFile(path.string(),
			aiProcess_GenUVCoords |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_FindInvalidData |
			aiProcess_FindDegenerates |
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_ImproveCacheLocality |
			aiProcess_ValidateDataStructure |
			aiProcess_JoinIdenticalVertices |
			aiProcess_RemoveRedundantMaterials |
			0
		);

		errorMessage = importer.GetErrorString();
		if (errorMessage == "") {

			if (assimpScene->HasMeshes()) {

				for (Index objectID = 0; objectID < assimpScene->mRootNode->mNumChildren; objectID++) {

					if (assimpScene->mRootNode->mChildren[objectID]->mNumMeshes > 0) {

						//get all meshes of object
						UInt numMeshes = assimpScene->mRootNode->mChildren[objectID]->mNumMeshes;

						CPUMesh mesh;
						mesh.name = String(assimpScene->mRootNode->mChildren[objectID]->mName.C_Str());

						UInt lastIndex = 0;

						for (UInt objectsMeshID = 0; objectsMeshID < numMeshes; objectsMeshID++) {
							Index meshID = assimpScene->mRootNode->mChildren[objectID]->mMeshes[objectsMeshID];

							aiColor4D diffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
							aiGetMaterialColor(assimpScene->mMaterials[assimpScene->mMeshes[meshID]->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);

							Vec3 color(diffuseColor.r, diffuseColor.g, diffuseColor.b);

							logDebug(color);

							if (assimpScene->mMeshes[meshID]->HasPositions()) {
								for (UInt i = 0; i < assimpScene->mMeshes[meshID]->mNumVertices; i++) {
									mesh.primitiveMode = PrimitiveMode::Triangles;

									Vec3 vertex;
									vertex.x = assimpScene->mMeshes[meshID]->mVertices[i].x;
									vertex.y = assimpScene->mMeshes[meshID]->mVertices[i].y;
									vertex.z = assimpScene->mMeshes[meshID]->mVertices[i].z;
									mesh.positions.pushBack(vertex);

									Vec2 texCoord = Vec2(0);
									if (assimpScene->mMeshes[meshID]->mTextureCoords[0]) {
										texCoord.x = assimpScene->mMeshes[meshID]->mTextureCoords[0][i].x;
										texCoord.y = assimpScene->mMeshes[meshID]->mTextureCoords[0][i].y;
									}
									mesh.textureCoordinates.pushBack(texCoord);

									Vec3 normal;
									normal.x = assimpScene->mMeshes[meshID]->mNormals[i].x;
									normal.y = assimpScene->mMeshes[meshID]->mNormals[i].y;
									normal.z = assimpScene->mMeshes[meshID]->mNormals[i].z;
									mesh.normals.pushBack(normal);

									mesh.vertexColors.pushBack(color);
								}

								if (assimpScene->mMeshes[meshID]->HasFaces()) {
									for (UInt i = 0; i < assimpScene->mMeshes[meshID]->mNumFaces; i++) {
										for (UInt j = 0; j < assimpScene->mMeshes[meshID]->mFaces->mNumIndices; j++) { //should always be 3 (0 -> 1 -> 2 <<)
											Index index = lastIndex + assimpScene->mMeshes[meshID]->mFaces[i].mIndices[j];
											mesh.indices.pushBack(index);
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
							mesh.loaded = false;
							break;
						}
						else {
							mesh.checkIntegrity();
							mesh.saveMeshFile(resourceManager);
						}
						meshes.pushBack(mesh);
					}
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
