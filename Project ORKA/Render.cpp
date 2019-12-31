
#include "Program.hpp"

void initializeRenderer(Renderer& renderer, GameSimulation& gameSimulation) {
	renderer.gameSimulation = &gameSimulation;
	for (int i = 0; i < 64; i++) {
		renderer.chunkRenderQueues.push_back(std::vector<ChunkRenderInfo>());
	}
};

void renderWindow(Window & window) {
	int vWidth, vHeight;
	glfwGetFramebufferSize(window.glfwWindow, &vWidth, &vHeight);
	renderFrame(window.renderer, vWidth, vHeight);
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
	glDepthFunc(GL_LEQUAL);

	//wireframe mode
	if (renderer.wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//transparency / alpha
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//point renderer
	//glEnable(GL_PROGRAM_POINT_SIZE);

	pocessCamera(renderer.camera, renderer.renderTime);

	//view matrix
	renderer.viewMatrix = renderer.camera.viewMatrix;

	//projection matrix
	renderer.projectionMatrix = glm::perspective(glm::radians(renderer.camera.fieldOfView), (float)width / (float)height, 0.001f, 1000.0f);

	if (renderer.meshSystem.cpuMeshes.size() > 0) {
		uploadNextMeshFromQueue(renderer.meshSystem);
	}

	renderWorld(renderer.gameSimulation->worldSystem, renderer);
	glUseProgram(0); // somehow gets rid of shader recompilation on nvidia cards
}

void renderSky(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void renderGizmo(ChunkRenderInfo & info, Renderer & renderer) {
	useShaderProgram(renderer.primitiveChunkShaderProgram);

	unsigned int meshIndex;
	getMeshIndexFromName(renderer.meshSystem, "gizmo", meshIndex);

	bindMesh(renderer.meshSystem, meshIndex);

	glUniform1f(renderer.primitiveChunkShaderProgram.timeID, renderer.renderTime.total());

	glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
	glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.vpMatrixID, 1, GL_FALSE, glm::value_ptr(vpMatrix));

	glm::mat4 mMatrix = glm::scale(glm::mat4(1), glm::vec3(0.05));
	glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.mMatrixID, 1, GL_FALSE, glm::value_ptr(mMatrix));

	glUniform3fv(renderer.primitiveChunkShaderProgram.chunkOffsetVectorID, 1, glm::value_ptr(info.chunkOffsetVector));

	renderMesh(renderer.meshSystem, meshIndex);

	unbindMesh();
	glUseProgram(0);
}

void renderEntities(ChunkRenderInfo& info, Renderer& renderer){
	info.chunk->mutex.lock();
	std::vector<Entity>& entities = info.chunk->entities;

	//for all entities
	for (unsigned int currentEntityindex = 0; currentEntityindex < entities.size(); currentEntityindex++) {

		useShaderProgram(renderer.primitiveChunkShaderProgram);

		glUniform1f(renderer.primitiveChunkShaderProgram.timeID, renderer.renderTime.total());

		//apply unifforms
		glm::vec4 worldOffset = glm::vec4(info.chunk->location, info.chunk->level);
		glUniform4fv(renderer.primitiveChunkShaderProgram.worldOffsetID, 1, glm::value_ptr(worldOffset));

		glm::mat4 vpMatrix = renderer.projectionMatrix * renderer.viewMatrix;
		glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.vpMatrixID, 1, GL_FALSE, glm::value_ptr(vpMatrix));

		renderer.gameSimulation->modelMatrixSystem.mutex.lock();
		glUniformMatrix4fv(renderer.primitiveChunkShaderProgram.mMatrixID, 1, GL_FALSE, glm::value_ptr(renderer.gameSimulation->modelMatrixSystem.modelMatrices[entities[currentEntityindex].indices[ModelMatrixComponentType]]));
		renderer.gameSimulation->modelMatrixSystem.mutex.unlock();

		glUniform3fv(renderer.primitiveChunkShaderProgram.chunkOffsetVectorID, 1, glm::value_ptr(info.chunkOffsetVector));

		unsigned int meshIndex;

		getMeshIndexFromRenderComponent(renderer, entities[currentEntityindex].indices[RenderComponentType], meshIndex);

		bindMesh(renderer.meshSystem, meshIndex);

		renderMesh(renderer.meshSystem, meshIndex);
		unbindMesh();
	}
	info.chunk->mutex.unlock();
}

void RenderThread(Window& window)
{
	window.renderer.renderTime.resetTime();

	glfwMakeContextCurrent(window.glfwWindow);

	glewExperimental = true;
	try {
		if (glewInit() != GLEW_OK) {
			throw std::exception("Failed to initialize GLEW!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugOutputCallback, 0);



	loadShader(window.renderer.vertexShader, GL_VERTEX_SHADER, "shaders/primitive chunk.vert");
	loadShader(window.renderer.fragmentShader, GL_FRAGMENT_SHADER, "shaders/primitive.frag");

	loadShaderProgram(window.renderer.primitiveChunkShaderProgram, window.renderer.vertexShader, window.renderer.fragmentShader);

	unloadShader(window.renderer.vertexShader);
	unloadShader(window.renderer.fragmentShader);

	startMeshLoaderThread(window.renderer.meshSystem);

	while (window.keepThreadRunning) {
		window.renderer.renderTime.processTime();
		processKeyboardInput(window);
		renderWindow(window);
		dynamicallyAdjustValue(window.renderer.worldSystemRenderDistance, window.renderer);
	}
	stopMeshLoaderThread(window.renderer.meshSystem);

	unloadShaderProgram(window.renderer.primitiveChunkShaderProgram);

}