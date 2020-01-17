
#include "Program.hpp"

void unbindMesh() {
	//disable attributes
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);
	//glDisableVertexAttribArray(4);
}

void unloadMesh(GPUMesh& gpuMesh) {
	//make unavailable for rendering
	gpuMesh.readyToRender = false;

	//delete buffers
	glDeleteBuffers(1, &gpuMesh.vertexBuffer);
	glDeleteBuffers(1, &gpuMesh.positionBuffer);
	glDeleteBuffers(1, &gpuMesh.uvBuffer);
	glDeleteBuffers(1, &gpuMesh.indexBuffer);

	//delete vertex array object
	glDeleteVertexArrays(1, &gpuMesh.vertexArrayObject);
}

void loadAllMeshes(MeshSystem& meshSystem)
{
	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / "objects"))
		addMeshFileToLoaderQueue(meshSystem, entry.path());

	//create hardcoded bounding box
	CPUMesh boundingBox;
	boundingBox.name = "bounding box";
	boundingBox.primitiveMode = GL_LINES;
	float bSize = 0.5;
	float bSize2 = -0.5;
	boundingBox.vertices.push_back(glm::vec3(bSize, bSize, bSize));
	boundingBox.vertices.push_back(glm::vec3(bSize, bSize, bSize2));
	boundingBox.vertices.push_back(glm::vec3(bSize, bSize2, bSize));
	boundingBox.vertices.push_back(glm::vec3(bSize, bSize2, bSize2));
	boundingBox.vertices.push_back(glm::vec3(bSize2, bSize, bSize));
	boundingBox.vertices.push_back(glm::vec3(bSize2, bSize, bSize2));
	boundingBox.vertices.push_back(glm::vec3(bSize2, bSize2, bSize));
	boundingBox.vertices.push_back(glm::vec3(bSize2, bSize2, bSize2));
	bSize += 0.5;
	bSize2 += 0.5;
	boundingBox.uvs.push_back(glm::vec2(bSize, bSize));
	boundingBox.uvs.push_back(glm::vec2(bSize, bSize));
	boundingBox.uvs.push_back(glm::vec2(bSize, bSize2));
	boundingBox.uvs.push_back(glm::vec2(bSize, bSize2));
	boundingBox.uvs.push_back(glm::vec2(bSize2, bSize));
	boundingBox.uvs.push_back(glm::vec2(bSize2, bSize));
	boundingBox.uvs.push_back(glm::vec2(bSize2, bSize2));
	boundingBox.uvs.push_back(glm::vec2(bSize2, bSize2));
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
	//MeshContainer singlePoint;
	//singlePoint.name = "point";
	//singlePoint.primitiveMode = GL_POINTS;
	//singlePoint.vertices.push_back(glm::vec3(0.0f));
	//singlePoint.uvs.push_back(glm::vec2(0.5f));
	//singlePoint.indices.push_back(0);
	//addMeshToUploadQueue(meshSystem, singlePoint);
}

int MeshLoaderThread(MeshSystem& meshSystem) {

	loadAllMeshes(meshSystem);

	std::cout << "hello" << std::endl;

	while (meshSystem.keepThreadRunning) {
		if (meshSystem.meshLoadQueue.size() > 0) {
			generateCPUMeshFromLoaderQueue(meshSystem);
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
	
	return 0;
}

void stopMeshLoaderThread(MeshSystem& meshSystem) {
	meshSystem.readyForUpload = false;
	meshSystem.keepThreadRunning = false;
	meshSystem.thread->join();

	meshSystem.cpuMeshes.clear();
	meshSystem.gpuMeshes.clear();
	meshSystem.meshLoadQueue.clear();
	meshSystem.renderComponentIndexToGPUMeshIndex.clear();

	for (unsigned int gpuMeshIndex = 0; gpuMeshIndex < meshSystem.gpuMeshes.size(); gpuMeshIndex++) {
		unloadMesh(meshSystem.gpuMeshes[gpuMeshIndex]);
	}
}

void startMeshLoaderThread(MeshSystem& meshSystem) {
	meshSystem.keepThreadRunning = true;
	meshSystem.thread = std::make_unique<std::thread>(MeshLoaderThread, std::ref(meshSystem));

	meshSystem.readyForUpload = true;
}

void uploadNextMeshFromQueue(MeshSystem& meshSystem) {

	CPUMesh& cpuMesh = meshSystem.cpuMeshes.front();
	GPUMesh gpuMesh;
	if (cpuMesh.readyToUploadToGPU) {

		gpuMesh.name = cpuMesh.name;
		gpuMesh.indexCount = cpuMesh.indices.size();
		gpuMesh.primitiveMode = cpuMesh.primitiveMode;



		//create vertex array
		glGenVertexArrays(1, &gpuMesh.vertexArrayObject);

		//create vertex buffer
		glGenBuffers(1, &gpuMesh.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, cpuMesh.vertices.size() * sizeof(glm::vec3), cpuMesh.vertices.data(), GL_STATIC_DRAW);

		//create matrix buffer
		glGenBuffers(1, &gpuMesh.positionBuffer);

		//create uv buffer
		glGenBuffers(1, &gpuMesh.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gpuMesh.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, cpuMesh.uvs.size() * sizeof(glm::vec2), cpuMesh.uvs.data(), GL_STATIC_DRAW);

		//create index buffer
		glGenBuffers(1, &gpuMesh.indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuMesh.indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cpuMesh.indices.size() * sizeof(unsigned int), cpuMesh.indices.data(), GL_STATIC_DRAW);


		//add gpu mesh to system
		meshSystem.gpuMeshes.push_back(gpuMesh);
		meshSystem.cpuMeshes.pop_front();
		meshSystem.gpuMeshes.back().readyToRender = true;
	}
}

void bindMesh(MeshSystem& meshSystem, Index meshIndex) {

	if (meshIndex != UINT_MAX && meshSystem.gpuMeshes[meshIndex].readyToRender) {

		GPUMesh& mesh = meshSystem.gpuMeshes[meshIndex];

		//vertex array object
		glBindVertexArray(mesh.vertexArrayObject);

		//vertecies
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//positions
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.positionBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glVertexAttribDivisor(1, 1);

		//positions
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
	}
}

void renderMesh(MeshSystem& meshSystem, Index meshIndex) {
	if (meshIndex != UINT_MAX && meshSystem.gpuMeshes[meshIndex].readyToRender) {
		glDrawElements(
			meshSystem.gpuMeshes[meshIndex].primitiveMode,	// mode
			meshSystem.gpuMeshes[meshIndex].indexCount,		// count
			GL_UNSIGNED_INT,		// type
			(void*)0				// element array buffer offset
		);
	}
}

void generateCPUMeshFromLoaderQueue(MeshSystem& meshSystem) {

	//add new mesh to upload queue

	meshSystem.cpuMeshes.emplace_back();


	CPUMesh& mesh = meshSystem.cpuMeshes.back();

	//take info from file queue
	MeshFileInfo& info = meshSystem.meshLoadQueue.front();

	//create the mesh
	mesh.name = info.name;
	mesh.primitiveMode = info.primitiveMode;
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
	meshSystem.meshLoadQueue.pop_front();

	mesh.readyToUploadToGPU = true;
}

void addMeshToUploadQueue(MeshSystem& meshSystem, CPUMesh& mesh) {
	mesh.readyToUploadToGPU = false;
	meshSystem.cpuMeshes.push_back(mesh);
	meshSystem.cpuMeshes.back().readyToUploadToGPU = true;
}

void addMeshFileToLoaderQueue(MeshSystem& meshSystem, std::filesystem::path path) {
	if (path.extension() == ".fbx") {
		MeshFileInfo info;
		info.name = path.filename().string();
		info.name = info.name.erase(info.name.size() - 4);
		info.path = path.string();
		info.primitiveMode = GL_TRIANGLES;
		std::cout << "(" << info.name << ")		" << info.path << std::endl;
		meshSystem.meshLoadQueue.push_back(info);
	}
}

void getMeshIndexFromName(MeshSystem& meshSystem, const char* name, Index& meshIndex)
{
	for (int i = 0; i < meshSystem.gpuMeshes.size(); i++) {
		if (name == meshSystem.gpuMeshes[i].name) {
			meshIndex = i;
			return;
		}
	}
	meshIndex = UINT_MAX;
}

void renderInstancedMesh(MeshSystem& meshSystem, Index meshIndex, unsigned int numberOfInstances) {

	GPUMesh& mesh = meshSystem.gpuMeshes[meshIndex];

	if (meshIndex != UINT_MAX && mesh.readyToRender) {
		glDrawElementsInstanced(
			mesh.primitiveMode,		// mode
			mesh.indexCount,		// count
			GL_UNSIGNED_INT,		// type
			(void*)0,				// element array buffer offset
			numberOfInstances		// numberOfInstances
		);
	}
}

void getMeshIndexFromRenderComponent(Renderer& renderer, Index& inputRenderComponentID, Index& outputGPUMeshIndex) {

	//find mesh index related to render component
	std::map<Index, Index>::iterator it = renderer.meshSystem.renderComponentIndexToGPUMeshIndex.find(inputRenderComponentID);

	if (it != renderer.meshSystem.renderComponentIndexToGPUMeshIndex.end())
	{
		//return the mesh index
		outputGPUMeshIndex = it->second;
	}
	else {
		//did not find mesh index in table

		//lets try to manually find a mesh that has the same name
		//lets get the name first
		renderer.gameSimulation->renderComponentSystem.mutex.lock_shared();
		const char* name = renderer.gameSimulation->renderComponentSystem.names[inputRenderComponentID];
		renderer.gameSimulation->renderComponentSystem.mutex.unlock_shared();
		for (int gpuMeshIndex = 0; gpuMeshIndex < renderer.meshSystem.gpuMeshes.size(); gpuMeshIndex++) {
			if (renderer.meshSystem.gpuMeshes[gpuMeshIndex].name == name) {
				renderer.meshSystem.renderComponentIndexToGPUMeshIndex[inputRenderComponentID] = gpuMeshIndex;
				outputGPUMeshIndex = gpuMeshIndex;
				return;
			}
		}
		//did not find mesh that is uploaded
		//lets try to find the mesh on the disk
		outputGPUMeshIndex = UINT_MAX;
	}
}