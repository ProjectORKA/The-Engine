
#include "Program.hpp"

void getMeshIndexFromName(MeshSystem & meshSystem, std::string meshName, int & meshIndex) {

	meshIndex = -1;

	//find the name
	for (int i = 0; i < meshSystem.meshCount; i++) {
		if (meshSystem.names[i] == meshName) {
			meshIndex = i;
		}
	}

	//if (meshIndex == -1) {
	//	std::cout << "A mesh with the name " << meshName << " doesent exist!" << std::endl;
	//}
}

void uploadNextMeshFromQueue(MeshSystem & meshSystem) {
	MeshContainer & mesh = meshSystem.meshQueue.front();
	if (mesh.uploadable) {
	//update mesh count
	meshSystem.meshCount++;
	//add name
	meshSystem.names.push_back(mesh.name);
	//add loaded variable
	meshSystem.loaded.push_back(false);
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

	meshSystem.meshQueue.pop_front();

	meshSystem.loaded.back() = true;
	}
}
void unloadMesh(MeshSystem & meshSystem, int meshIndex) {
	glDeleteBuffers(1, &meshSystem.vertexBuffer[meshIndex]);
	glDeleteBuffers(1, &meshSystem.positionBuffer[meshIndex]);
	glDeleteBuffers(1, &meshSystem.uvBuffer[meshIndex]);

	glDeleteBuffers(1, &meshSystem.indexBuffer[meshIndex]);

	//delete vertex array object
	glDeleteVertexArrays(1, &meshSystem.vertexArrayObject[meshIndex]);

	meshSystem.loaded[meshIndex] = false;
}
void bindMesh(MeshSystem & meshSystem, int meshIndex) {
	if (meshIndex != -1 && meshSystem.loaded[meshIndex]) {
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
	if (meshIndex != -1 && meshSystem.loaded[meshIndex]) {
		glDrawElements(
			meshSystem.primitiveMode[meshIndex],	// mode
			meshSystem.indexCount[meshIndex],		// count
			GL_UNSIGNED_INT,						// type
			(void*)0								// element array buffer offset
		);
	}
}
void renderInstancedMesh(MeshSystem & meshSystem, int meshIndex, unsigned int numberOfInstances) {
	if (meshIndex != -1 && meshSystem.loaded[meshIndex]) {
		
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

void createMeshFromFile(MeshSystem & meshSystem) {
	//add new mesh to upload queue
	meshSystem.meshQueue.push_back(MeshContainer());
	MeshContainer & mesh = meshSystem.meshQueue.back();
	
	//take info from file queue
	MeshFileInfo & info = meshSystem.loaderQueue.front();

	//create the mesh
	mesh.name = info.name;
	mesh.primitiveMode = info.primitiveMode;
	Assimp::Importer Importer;
	const aiScene * scene = Importer.ReadFile(info.path,
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
	if (error) std::cout << "The " << info.name << " model could not be loaded. (" << info.path << ")" << std::endl;
	
	//remove entry from loader queue
	meshSystem.loaderQueue.pop_front();

	mesh.uploadable = true;

}
void loadAllMeshes(MeshSystem & meshSystem)
{
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/tree trunk.fbx", "tree");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/tree leaves.fbx", "tree");

	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/cube.fbx", "cube");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/plane.fbx", "plane");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/error.fbx", "error");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/triangle.fbx", "triangle");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/icosphere.fbx", "icosphere");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/ground plane.fbx", "terrain");
	addMeshFileToLoaderQueue(meshSystem, GL_TRIANGLES, "objects/suzanne.fbx", "monkey");
	addMeshFileToLoaderQueue(meshSystem, GL_POINTS   , "objects/suzanne high detail.fbx", "point cloud");

	//create hardcoded bounding box
	MeshContainer boundingBox;
	boundingBox.name = "bounds";
	boundingBox.primitiveMode = GL_LINES;
	boundingBox.vertices.push_back(glm::vec3(+1.0f, +1.0f, +1.0f));
	boundingBox.vertices.push_back(glm::vec3(+1.0f, +1.0f, -1.0f));
	boundingBox.vertices.push_back(glm::vec3(+1.0f, -1.0f, +1.0f));
	boundingBox.vertices.push_back(glm::vec3(+1.0f, -1.0f, -1.0f));
	boundingBox.vertices.push_back(glm::vec3(-1.0f, +1.0f, +1.0f));
	boundingBox.vertices.push_back(glm::vec3(-1.0f, +1.0f, -1.0f));
	boundingBox.vertices.push_back(glm::vec3(-1.0f, -1.0f, +1.0f));
	boundingBox.vertices.push_back(glm::vec3(-1.0f, -1.0f, -1.0f));
	boundingBox.indices.push_back(7);
	boundingBox.indices.push_back(3);
	boundingBox.indices.push_back(7);
	boundingBox.indices.push_back(5);
	boundingBox.indices.push_back(7);
	boundingBox.indices.push_back(6);
	boundingBox.indices.push_back(1);
	boundingBox.indices.push_back(3);
	boundingBox.indices.push_back(1);
	boundingBox.indices.push_back(5);
	boundingBox.indices.push_back(1);
	boundingBox.indices.push_back(0);
	boundingBox.indices.push_back(2);
	boundingBox.indices.push_back(6);
	boundingBox.indices.push_back(2);
	boundingBox.indices.push_back(0);
	boundingBox.indices.push_back(2);
	boundingBox.indices.push_back(3);
	boundingBox.indices.push_back(4);
	boundingBox.indices.push_back(6);
	boundingBox.indices.push_back(4);
	boundingBox.indices.push_back(5);
	boundingBox.indices.push_back(4);
	boundingBox.indices.push_back(0);
	addMeshToUploadQueue(meshSystem, boundingBox);

	//create hardcoded point mesh
	MeshContainer singlePoint;
	singlePoint.name = "point";
	singlePoint.primitiveMode = GL_POINTS;
	singlePoint.vertices.push_back(glm::vec3(0.0f));
	singlePoint.uvs.push_back(glm::vec2(0.5f));
	singlePoint.indices.push_back(0);
	addMeshToUploadQueue(meshSystem, singlePoint);
}
void MeshLoaderThread(MeshSystem & meshSystem) {
	debugPrint("Loader thread started.");

	while (meshSystem.keepThreadRunning) {
		if (meshSystem.loaderQueue.size() > 0) {
			createMeshFromFile(meshSystem);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}
void addMeshFileToLoaderQueue(MeshSystem & meshSystem, unsigned short primitiveMode, const char * path, std::string name) {
	debugPrint("Adding mesh to loader queue.");
	MeshFileInfo info;
	info.name = name;
	info.path = path;
	info.primitiveMode = primitiveMode;
	meshSystem.loaderQueue.push_back(info);
}
void addMeshToUploadQueue(MeshSystem & meshSystem, MeshContainer & mesh) {
	mesh.uploadable = false;
	meshSystem.meshQueue.push_back(mesh);
	meshSystem.meshQueue.back().uploadable = true;
}
MeshSystem::MeshSystem()
{
	thread = std::make_unique<std::thread>(MeshLoaderThread, std::ref(*this));
}
MeshSystem::~MeshSystem()
{
	keepThreadRunning = false;
	thread->join();

	for (int i = 0; i < meshCount; i++) {
		unloadMesh(*this, i);
	}
}

