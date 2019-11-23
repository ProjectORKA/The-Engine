
#include "Program.hpp"




void renderWindow(Window & window) {
	int vWidth, vHeight;
	glfwGetFramebufferSize(window.glfwWindow, &vWidth, &vHeight);
	
	renderFrame(*window.renderer, vWidth, vHeight);
	
	glfwSwapBuffers(window.glfwWindow);
}

void renderFrame(Renderer & renderer, int width, int height) {
	if (!width || !height) { return; };
	glViewport(0, 0, width, height);

	//backface culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//depth testing
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//wireframe mode
	if (renderer.wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//transparency / alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//point renderer
	glEnable(GL_PROGRAM_POINT_SIZE);

	//view matrix
	renderer.viewMatrix = renderer.camera->viewMatrix;

	//projection matrix
	renderer.projectionMatrix = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.001f, 1000.0f);

	if (renderer.meshSystem.meshQueue.size() > 0) {
		uploadNextMeshFromQueue(renderer.meshSystem);
	}

	renderWorld(renderer.gameServer->worldSystem, renderer);
	glUseProgram(0); // somehow gets rid of shader recompilation on nvidia cards
}

void renderSky(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderEntities(EntityComponentSystem & ecs, Renderer & renderer) {
	//for all entity types
	for (int currentEntityTypeIndex = 0; currentEntityTypeIndex < ecs.entityTypes.numberOfEntityTypes; currentEntityTypeIndex++) {
		//check if it has entities
		if (ecs.entityTypes.structures[currentEntityTypeIndex][0] && ecs.entityTypes.entityArrays[currentEntityTypeIndex].size() > 0) {
			//render entities based on name
			useShader(renderer.primitiveShaderInstanced);

			std::vector<int> meshIndices;
			getMeshIndicesFromName(renderer.meshSystem, ecs.entityTypes.names[currentEntityTypeIndex], meshIndices);

			for(int i = 0; i < meshIndices.size(); i++) {
			
				bindMesh(renderer.meshSystem, meshIndices[i]);
				
				std::vector<glm::vec3> instancedPositions;

				for (int j = 0; j < ecs.entityTypes.entityArrays[currentEntityTypeIndex].size(); j++) {
					
					unsigned int transformationIndex = ecs.entityTypes.entityArrays[currentEntityTypeIndex][j].componentIndices[0];
					glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
					
					glUniformMatrix4fv(renderer.primitiveShaderInstanced.vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
					
					instancedPositions.push_back(ecs.transformationSystem.transformations[transformationIndex].location);
				}

				glBindBuffer(GL_ARRAY_BUFFER, renderer.meshSystem.positionBuffer[meshIndices[i]]);
				glBufferData(GL_ARRAY_BUFFER, instancedPositions.size() * sizeof(glm::vec3), instancedPositions.data(), GL_STREAM_DRAW);

				renderInstancedMesh(renderer.meshSystem, meshIndices[i], instancedPositions.size());

				unbindMesh();
			}

		}
	}
}

void renderWorld(WorldSystem & worldSystem, Renderer & renderer) {
	renderSky(worldSystem.sky);

	renderChunkQueue(renderer);

	processChunkQueue(worldSystem.root, renderer);
	
}

void processChunkQueue(Chunk & chunk, Renderer & renderer) {
	//get location of chunk
	unsigned short levelBasedBitShift = 64 - chunk.level;
	unsigned long long offset = ULLONG_MAX >> (chunk.level + 1);

	long long x = chunk.location.x << levelBasedBitShift;	//converts to actual position
	long long y = chunk.location.y << levelBasedBitShift;	//converts to actual position
	long long z = chunk.location.z << levelBasedBitShift; 	//converts to actual position

	Camera & camera = *renderer.camera;
	//subtract location of camera to get delta/distance
	x -= camera.location.x - offset;
	y -= camera.location.y - offset;
	z -= camera.location.z - offset;

	//turn integer to float
	glm::vec3 delta(x, y, z);

	//normalize and imitate scale by moving it further away instead of scaling (allows for constant clipping)
	delta /= pow(2, 64 - chunk.level);

	//check visibility
	if (glm::length(delta) < VIEW_DISTANCE & chunk.level != CHUNK_LEVEL_MAX & chunk.hasContents) {
		if (!chunk.subdivided) {
			subdivideChunk(chunk);
		}
		processChunkQueue(*chunk.tfr, renderer);
		processChunkQueue(*chunk.tfl, renderer);
		processChunkQueue(*chunk.tbr, renderer);
		processChunkQueue(*chunk.tbl, renderer);
		processChunkQueue(*chunk.bfr, renderer);
		processChunkQueue(*chunk.bfl, renderer);
		processChunkQueue(*chunk.bbr, renderer);
		processChunkQueue(*chunk.bbl, renderer);
	}
	else {
		//apply delta to view matrix
		delta -= (camera.subChunkLocation / (float)pow(2, 64 - chunk.level));
		//add to render queue
		ChunkRenderInfo info;
		info.chunk = &chunk;
		info.offsetMatrix = glm::translate(glm::mat4(1), delta);
		renderer.chunkRenderQueues[chunk.level].push_back(info);
	}
}

void renderGizmo(ChunkRenderInfo & info, Renderer & renderer) {
	useShader(renderer.primitiveShader);

	std::vector<int> meshIndices;
	getMeshIndicesFromName(renderer.meshSystem, "gizmo", meshIndices);
	for (int i = 0; i < meshIndices.size(); i++) {

		bindMesh(renderer.meshSystem, meshIndices[i]);

		glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
		glm::mat4 mMatrix = glm::scale(info.offsetMatrix, glm::vec3(0.05));
		glUniformMatrix4fv(renderer.primitiveShader.mMatrixID, 1, GL_FALSE, &mMatrix[0][0]);
		glUniformMatrix4fv(renderer.primitiveShader.vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);

		renderMesh(renderer.meshSystem, meshIndices[i]);

		unbindMesh();
	}
	glUseProgram(0);
}

//rendering
void renderChunkQueue(Renderer & renderer) {
	for (int level = 0; level < renderer.chunkRenderQueues.size(); level++) {
		for (int chunk = 0; chunk < renderer.chunkRenderQueues[level].size(); chunk++) {
			renderChunk(renderer.chunkRenderQueues[level][chunk], renderer);
			if (level == 2) renderGizmo(renderer.chunkRenderQueues[level][chunk], renderer);
		}
		renderer.chunkRenderQueues[level].clear();
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

void renderChunk(ChunkRenderInfo & info, Renderer & renderer) {
	if (info.chunk->level == 1) {
		renderGizmo(info, renderer);
	}
	//render chunk
	renderChunkBoundingBox(info, renderer);
}

void renderChunkBoundingBox(ChunkRenderInfo & info, Renderer & renderer) {
	//use shader
	useShader(renderer.primitiveChunk);

	//apply uniforms
	glm::vec4 worldOffset = glm::vec4(info.chunk->location, info.chunk->level);
	glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
	glUniform4fv(renderer.primitiveChunk.worldOffsetID, 1, glm::value_ptr(worldOffset));
	glUniformMatrix4fv(renderer.primitiveChunk.mMatrixID, 1, GL_FALSE, &info.offsetMatrix[0][0]);
	glUniformMatrix4fv(renderer.primitiveChunk.vpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);

	//render meshes
	std::vector<int> meshIndices;
	if (renderer.chunkBorders) {
		getMeshIndicesFromName(renderer.meshSystem, "bounds", meshIndices);
		for (int i = 0; i < meshIndices.size(); i++) {
			bindMesh(renderer.meshSystem, meshIndices[i]);
			renderMesh(renderer.meshSystem, meshIndices[i]);
			unbindMesh();
		}
	}
	if (info.chunk->location.z == 0) {
		getMeshIndicesFromName(renderer.meshSystem, "bounding plane", meshIndices);
		for (int i = 0; i < meshIndices.size(); i++) {
			bindMesh(renderer.meshSystem, meshIndices[i]);
			renderMesh(renderer.meshSystem, meshIndices[i]);
			unbindMesh();
		}
	}

	glUseProgram(0);
}




Renderer::Renderer(GameServer & gameServer, Camera & camera)
{

	this->gameServer = &gameServer;
	this->camera = &camera;
	for (int i = 0; i < 64; i++) {
		chunkRenderQueues.push_back(std::vector<ChunkRenderInfo>());
	}



	projectionMatrix = glm::mat4(1);
	viewMatrix = glm::mat4(1);

	debugPrint("|---RenderingSystem was created!");
}
Renderer::~Renderer()
{
	debugPrint("|---RenderingSystem was destroyed!");
}
