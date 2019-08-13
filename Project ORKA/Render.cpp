
#include "Program.hpp"

void renderWindow(Window & window) {
	int vWidth, vHeight;
	glfwGetFramebufferSize(window.glfwWindow, &vWidth, &vHeight);
	window.renderer->viewMatrix = window.camera.viewMatrices;
	renderFrame(*window.renderer, vWidth, vHeight);
	glfwSwapBuffers(window.glfwWindow);
}

void renderFrame(Renderer & renderer, int width, int height) {
	if (!width || !height) { return; };
	glViewport(0, 0, width, height);

	//backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//depth testing
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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
	//glBlendEquation(GL_FUNC_ADD);
	
	//point renderer
	//glPointSize(10.0f);
	glEnable(GL_PROGRAM_POINT_SIZE);

	//projection matrix
	renderer.projectionMatrix = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 20000.0f);

	renderWorld(renderer.gameServer->worldSystem, renderer);
	glUseProgram(0); // somehow gets rid of shader recompilation on nvidia cards
}

void renderWorld(WorldSystem & worldSystem, Renderer & renderer) {
	renderSky(worldSystem.sky);
	renderChunk(worldSystem.chunk, renderer);
	renderChunkBoundingBox(worldSystem.chunk, renderer);
}

void renderSky(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderChunkBoundingBox(Chunk & chunk, Renderer & renderer) {
	useShader(renderer.primitiveShader);

	int meshIndex;
	getMeshIndexFromName(renderer.meshSystem, "bounds", meshIndex);

	bindMesh(renderer.meshSystem, meshIndex);

	glm::mat4 modelMatrix = glm::scale(glm::mat4(1), glm::vec3(5));
	glm::mat4 mvpMatrix = renderer.projectionMatrix * renderer.viewMatrix * modelMatrix;
	glUniformMatrix4fv(renderer.primitiveShader.mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

	renderMesh(renderer.meshSystem, meshIndex);

	unbindMesh();
}

void renderChunk(Chunk & chunk, Renderer & renderer) {
	renderEntities(chunk.entityComponentSystem, renderer);
}

void getMeshIndexOfEntityType(std::string entityTypeName, std::vector<std::string> & meshNames, int & meshIndex) {
	for (int i = 0; i < meshNames.size(); i++) {
		if (meshNames[i] == entityTypeName) {
			meshIndex = i;
			return;
		}
	}
}

void renderEntities(EntityComponentSystem & ecs, Renderer & renderer) {
	//for all entity types
	
	for (int i = 0; i < ecs.entityTypes.numberOfEntityTypes; i++) {
		//check if it has entities
		if (ecs.entityTypes.structure[i][0] && ecs.entityTypes.entityArrays[i].size() > 0) {
			//render entities based on name
			useShader(renderer.primitiveShaderInstanced);

			int meshIndex;
			getMeshIndexFromName(renderer.meshSystem, ecs.entityTypes.names[i], meshIndex);

			if (meshIndex != -1) {
			
				bindMesh(renderer.meshSystem, meshIndex);
				
				std::vector<glm::vec3> instancedPositions;

				for (int j = 0; j < ecs.entityTypes.entityArrays[i].size(); j++) {
					unsigned int transformationIndex = ecs.entityTypes.entityArrays[i][j].componentIndices[0];
					glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
					
					glUniformMatrix4fv(renderer.primitiveShaderInstanced.mvpMatrixID, 1, GL_FALSE, &vpMatrix[0][0]);
					
					instancedPositions.push_back(ecs.transformationSystem.transformations[transformationIndex].location);
				}

				glBindBuffer(GL_ARRAY_BUFFER, renderer.meshSystem.positionBuffer[meshIndex]);
				glBufferData(GL_ARRAY_BUFFER, instancedPositions.size() * sizeof(glm::vec3), instancedPositions.data(), GL_STREAM_DRAW);

				renderInstancedMesh(renderer.meshSystem, meshIndex, instancedPositions.size());

				unbindMesh();
			}
		}
	}
}

Renderer::Renderer(GameServer & gameServer)
{
	this->gameServer = &gameServer;

	projectionMatrix = glm::mat4(1);
	viewMatrix = glm::mat4(1);

	debugPrint("|---RenderingSystem was created!");
}
Renderer::~Renderer()
{
	debugPrint("|---RenderingSystem was destroyed!");
}
