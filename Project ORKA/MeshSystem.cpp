
#include "Program.hpp"

void makeMeshAvaivable(Renderer & renderer, std::string name, const char * path) {

	//check if mesh has entry
	bool hasEntry = false;
	for (unsigned int meshIndex = 0; meshIndex < renderer.meshSystem.meshCount; meshIndex++) {
		if (renderer.meshSystem.names[meshIndex] == name) {

			//check if path is same
			if (renderer.meshSystem.paths[meshIndex] == path) {
				//check if loaded
				if (renderer.meshSystem.loaded[meshIndex]) {
					//mesh avaivable
					return;
				}
				else {
					//mesh needs to be loaded
					loadMesh(renderer, meshIndex);
				}
			}
			else {
				//path is not the same
				renderer.meshSystem.paths[meshIndex] = path;
				loadMesh(renderer, meshIndex);
			}
			hasEntry = true;
		}
	}
	if (!hasEntry) {
		renderer.meshSystem.meshCount++;
		renderer.meshSystem.names.push_back(name);
		renderer.meshSystem.paths.push_back(path);
		renderer.meshSystem.loaded.push_back(false);
		renderer.meshSystem.indexCount.push_back(0);
		renderer.meshSystem.vertexArrayObject.push_back(GLuint(0));
		renderer.meshSystem.vertexBuffer.push_back(GLuint(0));
		renderer.meshSystem.indexBuffer.push_back(GLuint(0));

		loadMesh(renderer, renderer.meshSystem.meshCount - 1);
	}
}

void loadMesh(Renderer & renderer, ComponentIndex meshIndex) {

	if (!renderer.meshSystem.loaded[meshIndex]) {

		if (renderer.meshSystem.paths[meshIndex] != nullptr) {

			//create temporary storage for usable data
			std::vector<unsigned int> indices;
			std::vector<glm::vec3> vertices;
			//std::vector<glm::vec3> normals;
			//std::vector<glm::vec2> UVs;

			//load file to assimp scene
			Assimp::Importer Importer;
			const aiScene * scene = Importer.ReadFile(renderer.meshSystem.paths[meshIndex],
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

							//glm::vec2 texCoord;
							//texCoord.x = scene->mMeshes[0]->mTextureCoords[0][i].x;
							//texCoord.y = scene->mMeshes[0]->mTextureCoords[0][i].y;
							//uvs.push_back(texCoord);

							//glm::vec3 normal;
							//normal.x = scene->mMeshes[0]->mNormals[i].x;
							//normal.y = scene->mMeshes[0]->mNormals[i].y;
							//normal.z = scene->mMeshes[0]->mNormals[i].z;
							//normals.push_back(normal);

							//glm::vec3 tangent;
							//tangent.x = scene->mMeshes[0]->mTangents->x;
							//tangent.y = scene->mMeshes[0]->mTangents->y;
							//tangent.z = scene->mMeshes[0]->mTangents->z;
							//tangents.push_back(tangent);
							//
							//glm::vec3 bitangent;
							//bitangent.x = scene->mMeshes[0]->mBitangents->x;
							//bitangent.y = scene->mMeshes[0]->mBitangents->y;
							//bitangent.z = scene->mMeshes[0]->mBitangents->z;
							//bitangents.push_back(bitangent);
						}
						if (scene->mMeshes[0]->HasFaces()) {
							indices.clear();
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
				renderer.meshSystem.loaded[meshIndex] = false;
				debugPrint("Could not load mesh from file!");
				return;
			}
			else {

				//Create Vertex Array Object
				glGenVertexArrays(1, &renderer.meshSystem.vertexArrayObject[meshIndex]);
				glBindVertexArray(renderer.meshSystem.vertexArrayObject[meshIndex]);

				//Create Buffers
				glGenBuffers(1, &renderer.meshSystem.vertexBuffer[meshIndex]);
				glBindBuffer(GL_ARRAY_BUFFER, renderer.meshSystem.vertexBuffer[meshIndex]);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

				//glGenBuffers(1, &uvBuffer);
				//glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
				//glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

				//glGenBuffers(1, &normalBuffer);
				//glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
				//glBufferData(GL_ARRAY_BUFFER, meshData.normals.size() * sizeof(glm::vec3), meshData.normals.data(), GL_STATIC_DRAW);

				//glGenBuffers(1, &tangentBuffer);
				//glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
				//glBufferData(GL_ARRAY_BUFFER, meshData.tangents.size() * sizeof(glm::vec3), meshData.tangents.data(), GL_STATIC_DRAW);
				//
				//glGenBuffers(1, &bitangentBuffer);
				//glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
				//glBufferData(GL_ARRAY_BUFFER, meshData.bitangents.size() * sizeof(glm::vec3), meshData.bitangents.data(), GL_STATIC_DRAW);

				glGenBuffers(1, &renderer.meshSystem.indexBuffer[meshIndex]);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.meshSystem.indexBuffer[meshIndex]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

				renderer.meshSystem.indexCount[meshIndex] = (unsigned int)indices.size();
				renderer.meshSystem.loaded[meshIndex] = true;
			}
		}
		else {
			debugPrint("No path for mesh avaivable!");
			system("pause");
		}
	}
}

void unloadMesh(MeshSystem & meshSystem, ComponentIndex meshIndex) {

	if (meshSystem.loaded[meshIndex]) {
		glDeleteBuffers(1, &meshSystem.vertexBuffer[meshIndex]);
		//glDeleteBuffers(1, &uvBuffer);
		//glDeleteBuffers(1, &normalBuffer);
		//glDeleteBuffers(1, &tangentBuffer);
		//glDeleteBuffers(1, &bitangentBuffer);

		glDeleteBuffers(1, &meshSystem.indexBuffer[meshIndex]);

		//delete vertex array object
		glDeleteVertexArrays(1, &meshSystem.vertexArrayObject[meshIndex]);

		meshSystem.indexCount[meshIndex] = 0;
		meshSystem.loaded[meshIndex] = false;
	}

}

void bindMesh(Renderer & renderer, unsigned int meshIndex) {
	if (renderer.meshSystem.loaded[meshIndex]) {

		glBindVertexArray(renderer.meshSystem.vertexArrayObject[meshIndex]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, renderer.meshSystem.vertexBuffer[meshIndex]);
		glVertexAttribPointer(
			0,                  // index must match layout in shader
			3,                  // size : x + y + z
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		//glEnableVertexAttribArray(1);
		//glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBuffer);
		//glVertexAttribPointer(
		//	1,                                // index
		//	2,                                // size : u + v
		//	GL_FLOAT,                         // type
		//	GL_FALSE,                         // normalized?
		//	0,                                // stride
		//	(void*)0                          // array buffer offset
		//);
		//// 3rd attribute buffer : normals
		//glEnableVertexAttribArray(2);
		//glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBuffer);
		//glVertexAttribPointer(
		//	2,                                // attribute
		//	3,                                // size
		//	GL_FLOAT,                         // type
		//	GL_FALSE,                         // normalized?
		//	0,                                // stride
		//	(void*)0                          // array buffer offset
		//);
		// 4th attribute buffer : tangents
		//glEnableVertexAttribArray(3);
		//glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
		//glVertexAttribPointer(
		//	3,                                // attribute
		//	3,                                // size
		//	GL_FLOAT,                         // type
		//	GL_FALSE,                         // normalized?
		//	0,                                // stride
		//	(void*)0                          // array buffer offset
		//);
		//// 5th attribute buffer : bitangents
		//glEnableVertexAttribArray(4);
		//glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
		//glVertexAttribPointer(
		//	4,                                // attribute
		//	3,                                // size
		//	GL_FLOAT,                         // type
		//	GL_FALSE,                         // normalized?
		//	0,                                // stride
		//	(void*)0                          // array buffer offset
		//);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer.meshSystem.indexBuffer[meshIndex]);
	}
	else {
		loadMesh(renderer, meshIndex);
	}
}

void renderMesh(Renderer & renderer, unsigned int index) {
	glDrawElements(
		GL_TRIANGLES,							// mode
		renderer.meshSystem.indexCount[index],	// count
		GL_UNSIGNED_INT,						// type
		(void*)0								// element array buffer offset
	);

}

void unbindMesh() {
	//disable attributes
	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);
	//glDisableVertexAttribArray(4);
}


MeshSystem::MeshSystem()
{
	meshCount = 0;
}

MeshSystem::~MeshSystem()
{
	for (int i = 0; i < meshCount; i++) {
		if (loaded[i]) {
			unloadMesh( *this , i);
		}
	}
}

