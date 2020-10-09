#include "WindowSystem.hpp"

void WindowSystem::start()
{
	assert(glfwInit() == GLFW_TRUE);
	glfwSetErrorCallback(whenWindowAPIThrowsError);
}

void WindowSystem::addWindow(GameSimulation & gameSimulation) {
	windows.emplace_back();
	windows.back().renderer.gameSimulation = &gameSimulation;
	windows.back().create();
}

void WindowSystem::processLoop(GameSimulation & gameSimulation) {
	while (windows.size() > 0) {

		glfwWaitEvents();

		for (auto it = windows.begin(); it != windows.end(); it++) {
			if (it->duplicateWindow) {
				addWindow(gameSimulation);
				it->duplicateWindow = false;
			}
			if (it->shouldClose()) {
				it->destroy();
				windows.erase(it);
				break;
			}
		}
	}
}

void WindowSystem::stop()
{
	windows.clear();
	glfwTerminate();
}

void whenWindowAPIThrowsError(Int error, const char* description)
{
	std::cout << "Error: " << description << "\n";
	std::getchar();
	exit(EXIT_FAILURE);
}