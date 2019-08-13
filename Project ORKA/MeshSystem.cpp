
#include "Program.hpp"

void createAndUploadMeshFromFile(MeshSystem & meshSystem, unsigned short primitiveMode, const char * path, std::string name) {
	MeshContainer mesh;
	createMeshFromFile(mesh, primitiveMode, path, name);
	uploadStaticMesh(meshSystem, mesh);
}

void createMeshFromFile(MeshContainer & mesh, unsigned short primitiveMode, const char * path, std::string name) {

	mesh.name = name;
	mesh.primitiveMode = primitiveMode;

	Assimp::Importer Importer;
	const aiScene * scene = Importer.ReadFile(path,
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
					mesh.vertices.push_back(vertex);

					glm::vec2 texCoord;
					texCoord.x = scene->mMeshes[0]->mTextureCoords[0][i].x;
					texCoord.y = scene->mMeshes[0]->mTextureCoords[0][i].y;
					mesh.uvs.push_back(texCoord);

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
					for (unsigned int i = 0; i < scene->mMeshes[0]->mNumFaces; i++) {
						for (unsigned int j = 0; j < scene->mMeshes[0]->mFaces->mNumIndices; j++) { //should always be 3 (0 -> 1 -> 2 <<)
							unsigned int index = scene->mMeshes[0]->mFaces[i].mIndices[j];
							mesh.indices.push_back(index);
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
	if (error) std::cout << "The " << name << " model could not be loaded. (" << path << ")" << std::endl;
}

void uploadStaticMesh(MeshSystem & meshSystem, MeshContainer & mesh) {
	//update mesh count
	meshSystem.meshCount++;
	//add name
	meshSystem.names.push_back(mesh.name);
	//save index count
	meshSystem.indexCount.push_back(mesh.indices.size());
	//save primitive mode
	meshSystem.primitiveMode.push_back(mesh.primitiveMode);


	//create vertex array object
	meshSystem.vertexArrayObject.push_back(GLuint(0));
	glGenVertexArrays(1, &meshSystem.vertexArrayObject.back());


	//create vertex buffer
	meshSystem.vertexBuffer.push_back(GLuint(0));
	glGenBuffers(1, &meshSystem.vertexBuffer.back());
	glBindBuffer(GL_ARRAY_BUFFER, meshSystem.vertexBuffer.back());
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(glm::vec3), mesh.vertices.data(), GL_STATIC_DRAW);

	//create matrix buffer
	meshSystem.positionBuffer.push_back(GLuint(0));
	glGenBuffers(1, &meshSystem.positionBuffer.back());

	//create uv buffer
	meshSystem.uvBuffer.push_back(GLuint(0));
	glGenBuffers(1, &meshSystem.uvBuffer.back());
	glBindBuffer(GL_ARRAY_BUFFER, meshSystem.uvBuffer.back());
	glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(glm::vec2), mesh.uvs.data(), GL_STATIC_DRAW);

	//create index buffer
	meshSystem.indexBuffer.push_back(GLuint(0));
	glGenBuffers(1, &meshSystem.indexBuffer.back());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshSystem.indexBuffer.back());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

}

void unloadMesh(MeshSystem & meshSystem, int meshIndex) {

	glDeleteBuffers(1, &meshSystem.vertexBuffer[meshIndex]);
	glDeleteBuffers(1, &meshSystem.positionBuffer[meshIndex]);
	glDeleteBuffers(1, &meshSystem.uvBuffer[meshIndex]);
	//glDeleteBuffers(1, &normalBuffer);
	//glDeleteBuffers(1, &tangentBuffer);
	//glDeleteBuffers(1, &bitangentBuffer);

	glDeleteBuffers(1, &meshSystem.indexBuffer[meshIndex]);

	//delete vertex array object
	glDeleteVertexArrays(1, &meshSystem.vertexArrayObject[meshIndex]);
}

void getMeshIndexFromName(MeshSystem & meshSystem, std::string meshName, int & meshIndex) {

	meshIndex = -1;

	//find the name
	for (int i = 0; i < meshSystem.meshCount; i++) {
		if (meshSystem.names[i] == meshName) {
			meshIndex = i;
		}
	}

	if (meshIndex == -1) {
		std::cout << "A mesh with the name " << meshName << " doesent exist!" << std::endl;
	}
}

void bindMesh(MeshSystem & meshSystem, int meshIndex) {
	if (meshIndex != -1) {
		//vertex array object
		glBindVertexArray(meshSystem.vertexArrayObject[meshIndex]);

		//vertecies
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, meshSystem.vertexBuffer[meshIndex]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//positions
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, meshSystem.positionBuffer[meshIndex]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(1, 1);

		//positions
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, meshSystem.uvBuffer[meshIndex]);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshSystem.indexBuffer[meshIndex]);
	}
}

void renderMesh(MeshSystem & meshSystem, int meshIndex) {
	if (meshIndex != -1) {
		glDrawElements(
			meshSystem.primitiveMode[meshIndex],	// mode
			meshSystem.indexCount[meshIndex],		// count
			GL_UNSIGNED_INT,						// type
			(void*)0								// element array buffer offset
		);
	}
}

void renderInstancedMesh(MeshSystem & meshSystem, int meshIndex, unsigned int numberOfInstances) {
	if (meshIndex != -1) {
		glDrawElementsInstanced(
			meshSystem.primitiveMode[meshIndex],	// mode
			meshSystem.indexCount[meshIndex],		// count
			GL_UNSIGNED_INT,						// type
			(void*)0,								// element array buffer offset
			numberOfInstances						// numberOfInstances
		);
	}
}

void unbindMesh() {
	//disable attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
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
		unloadMesh(*this, i);
	}
}

