
#include "RenderingSystem.hpp"

void __stdcall DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		printf("OpenGL Debug Output message : ");

		if (source == GL_DEBUG_SOURCE_API_ARB)					printf("Source : API; ");
		else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	printf("Source : WINDOW_SYSTEM; ");
		else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	printf("Source : SHADER_COMPILER; ");
		else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		printf("Source : THIRD_PARTY; ");
		else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		printf("Source : APPLICATION; ");
		else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			printf("Source : OTHER; ");

		if (type == GL_DEBUG_TYPE_ERROR_ARB)					printf("Type : ERROR; ");
		else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	printf("Type : DEPRECATED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	printf("Type : UNDEFINED_BEHAVIOR; ");
		else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			printf("Type : PORTABILITY; ");
		else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			printf("Type : PERFORMANCE; ");
		else if (type == GL_DEBUG_TYPE_OTHER_ARB)				printf("Type : OTHER; ");

		if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				printf("Severity : HIGH; ");
		else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		printf("Severity : MEDIUM; ");
		else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			printf("Severity : LOW; ");
		printf("Message : %s\n", message);
		system("pause");
	}
};

RenderingSystem::RenderingSystem(GameServer & gameWorld) : gameServer(gameWorld) {

}

void RenderingSystem::render() {
	////matrices stuff
	//	// -> camera settings stuff
	//glm::mat4 projectionMatrix;
	//
	//	// -> camera transformation stuff
	//glm::mat4 viewMatrix;
	//
	//	// -> model Stuff
	//glm::mat4 modelMatrix;
	//projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
	renderWorld(gameServer.worldSystem);
}
void RenderingSystem::renderWorld(WorldSystem & worldSystem) {
	renderSky(worldSystem.sky);
	renderChunks(worldSystem.chunk);
}
void RenderingSystem::renderChunks(Chunk & chunk) {
	renderEntities(chunk.entities);
}
void RenderingSystem::renderSky(Sky & sky) {
	glClearColor(sky.skyColor.r, sky.skyColor.g, sky.skyColor.b, 0.75f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void RenderingSystem::renderEntities(std::vector<Entity *> & entities) {
	for (int i = 0; i < entities.size(); i++) {
		renderEntity(*entities[i]);
	}
}
void RenderingSystem::renderEntity(Entity & entity) {
	if (entity.type == "triangle") {
		meshHandler.meshMap["triangle"].render();
	}
}
void RenderingSystem::adaptViewport(unsigned int width, unsigned int height) {
	glViewport(0, 0, width, height);
}
void RenderingSystem::setDebugMessageCallback() {
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugOutputCallback, 0);
}