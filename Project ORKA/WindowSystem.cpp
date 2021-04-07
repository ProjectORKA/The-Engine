#include "WindowSystem.hpp"

WindowSystem windowSystem;

WindowSystem::WindowSystem()
{
	assert(glfwInit() == GLFW_TRUE);
	glfwSetErrorCallback(whenWindowAPIThrowsError);
}

WindowSystem::~WindowSystem()
{
	windows.clear();
	glfwTerminate();
}
void WindowSystem::processLoop() {
	while (windows.size() > 0) {
		glfwWaitEvents();
		for (auto it = windows.begin(); it != windows.end(); it++) {

			if (it->duplicateWindow) {
				addWindow();
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
void WindowSystem::addWindow() {
	windows.emplace_back();
	windows.back().create();
}

void whenWindowAPIThrowsError(Int error, const char* description)
{
	logError(description);
}