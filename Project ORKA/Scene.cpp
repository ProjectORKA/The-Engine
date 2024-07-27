#include "Scene.hpp"
#include "FileSystem.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

void Scene::loadFBX(Path path)
{
	// check if file is valid
	String errorMessage = "";

	if(doesPathExist(getDirectory(path)))
	{
		logDebug("Loading mesh: (" + path.stem().string() + ") from (" + path.string() + ")");

		Assimp::Importer importer;
		const aiScene*   assimpScene = importer.ReadFile(path.string(),
		                                                 // aiProcess_GenUVCoords |
		                                                 aiProcess_Triangulate | 0
		                                                 // aiProcess_SortByPType |
		                                                 // aiProcess_FindInvalidData |
		                                                 // aiProcess_FindDegenerates |
		                                                 // aiProcess_CalcTangentSpace |
		                                                 // aiProcess_GenSmoothNormals |
		                                                 // aiProcess_ImproveCacheLocality |
		                                                 // aiProcess_ValidateDataStructure |
		                                                 // aiProcess_JoinIdenticalVertices |
		                                                 // aiProcess_RemoveRedundantMaterials
		);

		errorMessage = importer.GetErrorString();
		if(errorMessage.empty())
		{
			if(assimpScene->HasMeshes())
			{
				for(Index objectId = 0; objectId < assimpScene->mRootNode->mNumChildren; objectId++)
				{
					if(assimpScene->mRootNode->mChildren[objectId]->mNumMeshes > 0)
					{
						// get all meshes of object
						UInt numMeshes = assimpScene->mRootNode->mChildren[objectId]->mNumMeshes;

						CpuMesh mesh;
						mesh.name = assimpScene->mRootNode->mChildren[objectId]->mName.C_Str();

						UInt lastIndex = 0;

						for(UInt objectsMeshId = 0; objectsMeshId < numMeshes; objectsMeshId++)
						{
							Index meshId = assimpScene->mRootNode->mChildren[objectId]->mMeshes[objectsMeshId];

							aiColor4D diffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
							aiGetMaterialColor(assimpScene->mMaterials[assimpScene->mMeshes[meshId]->mMaterialIndex], AI_MATKEY_COLOR_DIFFUSE, &diffuseColor);

							Vec3 color(diffuseColor.r, diffuseColor.g, diffuseColor.b);

							logDebug(color);

							if(assimpScene->mMeshes[meshId]->HasPositions())
							{
								for(UInt i = 0; i < assimpScene->mMeshes[meshId]->mNumVertices; i++)
								{
									mesh.primitiveMode = PrimitiveMode::Triangles;

									Vec3 vertex;
									vertex.x = assimpScene->mMeshes[meshId]->mVertices[i].x;
									vertex.y = assimpScene->mMeshes[meshId]->mVertices[i].y;
									vertex.z = assimpScene->mMeshes[meshId]->mVertices[i].z;
									mesh.positions.push_back(vertex);

									auto texCoord = Vec2(0);
									if(assimpScene->mMeshes[meshId]->mTextureCoords[0])
									{
										texCoord.x = assimpScene->mMeshes[meshId]->mTextureCoords[0][i].x;
										texCoord.y = assimpScene->mMeshes[meshId]->mTextureCoords[0][i].y;
									}
									mesh.textureCoordinates.push_back(texCoord);

									Vec3 normal;
									normal.x = assimpScene->mMeshes[meshId]->mNormals[i].x;
									normal.y = assimpScene->mMeshes[meshId]->mNormals[i].y;
									normal.z = assimpScene->mMeshes[meshId]->mNormals[i].z;
									mesh.normals.push_back(normal);

									mesh.vertexColors.push_back(color);
								}

								if(assimpScene->mMeshes[meshId]->HasFaces())
								{
									for(UInt i = 0; i < assimpScene->mMeshes[meshId]->mNumFaces; i++)
									{
										for(UInt j = 0; j < assimpScene->mMeshes[meshId]->mFaces->mNumIndices; j++)
										{
											// should always be 3 (0 -> 1 -> 2 <<)
											Index index = lastIndex + assimpScene->mMeshes[meshId]->mFaces[i].mIndices[j];
											mesh.indices.push_back(index);
										}
									}
								}
								else errorMessage = "Mesh does not have faces!";
							}
							else errorMessage = "Mesh does not have positions stored!";

							lastIndex += assimpScene->mMeshes[meshId]->mNumVertices;
						}

						if(!errorMessage.empty()) logError("The model (" + path.stem().string() + ") could not be loaded! (" + path.string() + ") Error: " + errorMessage);
						mesh.checkIntegrity();
						mesh.saveMeshFile();
						meshes.push_back(mesh);
					}
				}
			}
			else errorMessage = "No meshes in fbx scene!";
		}
	}
	else errorMessage = path.parent_path().string() + " does not exist!";

	if(!errorMessage.empty()) logError("The scene (" + path.stem().string() + ") could not be loaded! (" + path.string() + ") Error: " + errorMessage);
}
