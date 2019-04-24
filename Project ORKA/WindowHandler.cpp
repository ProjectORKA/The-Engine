
#include "WindowHandler.hpp"

WindowHandler::WindowHandler() {
	debugPrint("Initializing GLFW...");
	try {
		if (glfwInit() != GLFW_TRUE) {
			throw std::exception("Failed to initialize GLFW!");
		}
	}
	catch (std::exception error) {
		std::cout << "Error: " << error.what() << std::endl;
		std::getchar();
		exit(EXIT_FAILURE);
	};

	glfwSetErrorCallback(whenGLFWThrowsError);
}
WindowHandler::~WindowHandler() {
	glfwTerminate();
}
void WindowHandler::addWindow(RenderingSystem * renderingSystem) {
	debugPrint("Adding Window...");
	Window * tmp = new Window(renderingSystem);
	windows.push_back(tmp);
}
void WindowHandler::update() {
	glfwPollEvents();
	for (int i = 0; i < windows.size(); i++) {
		windows[i]->inputs();
		if (glfwWindowShouldClose(windows[i]->glfwWindow)) {
			delete windows[i];
			windows.erase(windows.begin() + i);
		}
	}
};
void WindowHandler::render() {
	for (int i = 0; i < windows.size(); i++) {
		windows[i]->render();
	}

}

//callbacks
void whenGLFWThrowsError(int error, const char* description)
{
	std::cout << "Error: " << description << std::endl;
	std::getchar();
	exit(EXIT_FAILURE);
}