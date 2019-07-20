
#include "Program.hpp"

void renderWindow(Window & window) {
	int vWidth, vHeight;
	glfwGetFramebufferSize(window.glfwWindow, &vWidth, &vHeight);
	window.renderer->viewMatrix = window.camera.viewMatrices;
	renderFrame(*window.renderer, vWidth, vHeight);
	glfwSwapBuffers(window.glfwWindow);
}

void renderFrame(Renderer & renderingSystem, int width, int height) {
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
	if (renderingSystem.wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//projection matrix
	renderingSystem.projectionMatrix = glm::perspective(glm::radians(80.0f), (float)width / (float)height, 0.1f, 10000.0f);

	renderWorld(renderingSystem.gameServer->worldSystem, renderingSystem);
	glUseProgram(0); // somehow gets rid of shader recompilation on nvidia cards
}

void renderWorld(WorldSystem & worldSystem, Renderer & renderingSystem) {
	renderSky(worldSystem.sky);
	renderChunk(worldSystem.chunk, renderingSystem);
}

void renderSky(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderChunk(Chunk & chunk, Renderer & renderingSystem) {
	renderEntities(chunk.entityComponentSystem, renderingSystem);
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
		//check if it has rendering data
		int meshIndex = -1;
		getMeshIndexOfEntityType(ecs.entityTypes.names[i], renderer.meshSystem.names, meshIndex);

		if (ecs.entityTypes.entityArrays[i].size() > 0 && meshIndex != -1) { //if type has entities and can be rendered

			useShader(renderer.primitiveShader);

			bindMesh(renderer, meshIndex);

			for (int j = 0; j < ecs.entityTypes.entityArrays[i].size(); j++) {
				if (ecs.entityTypes.structure[i][0] == true) {
					unsigned int transformationIndex = ecs.entityTypes.entityArrays[i][j].componentIndices[0];

					glm::mat4 mvpMatrix = renderer.projectionMatrix * renderer.viewMatrix * ecs.transformationSystem.modelMatrices[transformationIndex];
					glUniformMatrix4fv(renderer.primitiveShader.mvpMatrixID, 1, GL_FALSE, &mvpMatrix[0][0]);

					renderMesh(renderer, meshIndex);

				}
			}
			unbindMesh();
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
