#include "WindowSystem.hpp"

WindowSystem windowSystem;

WindowSystem::WindowSystem()
{
	assert(glfwInit() == GLFW_TRUE);
	glfwSetErrorCallback(whenWindowAPIThrowsError);
	glfwSetMonitorCallback(whenMonitorChanged);

}
WindowSystem::~WindowSystem()
{
	for (Window& window : windows) {
		window.destroy();
	}
	windows.clear();
	glfwTerminate();
}

void WindowSystem::startMainLoop() {
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
Window & WindowSystem::addWindow() {
	windows.emplace_back();
	windows.back().create();
	return windows.back();
}