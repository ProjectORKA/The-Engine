#include "Program.hpp"

int RenderThread(Window& window)
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
		if (window.renderer.meshSystem.cpuMeshes.size() > 0) {
			uploadNextMeshFromQueue(window.renderer.meshSystem);
		}
		processKeyboardInput(window);
		renderWindow(window);
		dynamicallyAdjustValue(window.renderer.settings.worldSystemRenderDistance, window.renderer);
	}

	stopMeshLoaderThread(window.renderer.meshSystem);

	unloadShaderProgram(window.renderer.primitiveChunkShaderProgram);

	return 0;
}

void renderWindow(Window & window) {
	glfwGetFramebufferSize(window.glfwWindow, &window.renderer.newFrameBufferWidth, &window.renderer.newFrameBufferHeight);
	renderFrame(window.renderer);
	glfwSwapBuffers(window.glfwWindow);
}

void renderViewport(Renderer & renderer, Viewport & viewport) {
	if (!viewport.absoluteWidth || !viewport.absoluteHeight) {
		return; 
	}
	glViewport(viewport.absoluteX, viewport.absoluteY, viewport.absoluteWidth, viewport.absoluteHeight);

	//projection matrix
	renderer.projectionMatrix = glm::perspective(glm::radians(renderer.camera.fieldOfView), (float)viewport.absoluteWidth / (float)viewport.absoluteHeight, 0.001f, 1000.0f);

	renderWorld(renderer.gameSimulation->worldSystem, renderer);
}

void updateViewPort(Renderer & renderer, Viewport& viewport) {
	viewport.absoluteX = renderer.frameBufferWidth * viewport.relativeX;
	viewport.absoluteY = renderer.frameBufferHeight * viewport.relativeY;
	viewport.absoluteWidth = renderer.frameBufferWidth * viewport.relativeWidth;
	viewport.absoluteHeight = renderer.frameBufferHeight * viewport.relativeHeight;
}

void renderFrame(Renderer & renderer) {
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);

	if ((renderer.newFrameBufferWidth != renderer.frameBufferWidth) | (renderer.newFrameBufferHeight != renderer.frameBufferHeight)) {
		renderer.frameBufferWidth = renderer.newFrameBufferWidth;
		renderer.frameBufferHeight = renderer.newFrameBufferHeight;		
		for (int i = 0; i < renderer.viewports.size(); i++) {
			updateViewPort(renderer, renderer.viewports[i]);
		}
	}

	if (renderer.settings.backfaceCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	//depth testing
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//wireframe mode
	if (renderer.settings.wireframeMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//view matrix
	renderer.viewMatrix = renderer.camera.viewMatrix;

	pocessCamera(renderer.camera, renderer.renderTime);

	for (int i = 0; i < renderer.viewports.size(); i++) {
		renderViewport(renderer, renderer.viewports[i]);
	}

	glUseProgram(0);
}

void renderGizmo(ChunkRenderInfo & info, Renderer & renderer) {
	useShaderProgram(renderer.primitiveChunkShaderProgram);

	unsigned int meshIndex;
	getMeshIndexFromName(renderer.meshSystem, "gizmo", meshIndex);

	bindMesh(renderer.meshSystem, meshIndex);

	glUniform1i(renderer.primitiveChunkShaderProgram.distortionID, renderer.settings.distortion);

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

void renderEntities(ChunkRenderInfo& info, Renderer& renderer) {
		std::vector<Entity>& entities = info.chunk->entities;

		//for all entities
		for (unsigned int currentEntityindex = 0; currentEntityindex < entities.size(); currentEntityindex++) {

			useShaderProgram(renderer.primitiveChunkShaderProgram);

			glUniform1i(renderer.primitiveChunkShaderProgram.distortionID, renderer.settings.distortion);

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
}

void initializeRenderer(Renderer& renderer, GameSimulation& gameSimulation) {
	renderer.gameSimulation = &gameSimulation;

	renderer.viewports.emplace_back();
	
	for (int i = 0; i < 64; i++) {
		renderer.chunkRenderQueues.push_back(std::vector<ChunkRenderInfo>());
	}
};
