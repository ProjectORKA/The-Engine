
#include "Program.hpp"

void loadMesh(Mesh & mesh, const char * path) {
	
	debugPrint("Loading Mesh!");

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	//std::vector<glm::vec3> normals;
	//std::vector<glm::vec2> UVs;

	if (!mesh.loaded) {
		//load file
		Assimp::Importer Importer;
		const aiScene * scene = Importer.ReadFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_Triangulate
		);
		//check if file is valid
		if (!scene) {
			mesh.loaded = false;
			return;
		}

		if (scene->HasMeshes()) {
			if (scene->mMeshes[0]->HasPositions()) {
				vertices.clear();
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
			}
		}

		//Create Vertex Array Object
		glGenVertexArrays(1, &mesh.vertexArrayObject);
		glBindVertexArray(mesh.vertexArrayObject);

		//Create Buffers
		glGenBuffers(1, &mesh.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
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

		glGenBuffers(1, &mesh.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		mesh.indexCount = (unsigned int)indices.size();

		mesh.loaded = true;

		debugPrint("Loaded Mesh!");

	}
}

void renderMesh(Mesh & mesh) {
	if (!mesh.loaded) loadMesh(mesh, "objects/suzanne.fbx");
	glBindVertexArray(mesh.vertexArrayObject);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
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

	//bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);

	//draw
	glDrawElements(
		GL_TRIANGLES,		// mode
		mesh.indexCount,	// count
		GL_UNSIGNED_INT,	// type
		(void*)0			// element array buffer offset
	);

	//disable attributes
	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);
	//glDisableVertexAttribArray(4);
}

Mesh::Mesh()
{
	debugPrint("|-----Mesh was created!");
}

Mesh::~Mesh()
{
	if (loaded) {
		glDeleteBuffers(1, &vertexBuffer);
		//glDeleteBuffers(1, &uvBuffer);
		//glDeleteBuffers(1, &normalBuffer);
		//glDeleteBuffers(1, &tangentBuffer);
		//glDeleteBuffers(1, &bitangentBuffer);

		glDeleteBuffers(1, &indexBuffer);

		//delete vertex array object
		glDeleteVertexArrays(1, &vertexArrayObject);

		indexCount = 0;

		loaded = false;
	}
	debugPrint("|-----Mesh destroyed!");
}

MeshHandler::MeshHandler()
{
	meshMap["triangle"] = std::make_shared<Mesh>();
	debugPrint("|----MeshHandler was created!");
}

MeshHandler::~MeshHandler()
{
	debugPrint("|----MeshHandler was destroyed!");

}
