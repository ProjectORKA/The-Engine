#include "Program.hpp"

void createNewWindow(WindowHandler & windowHandler, GameServer & gameServer) {
	std::shared_ptr<Window> tmp = std::make_shared<Window>(gameServer);
	windowHandler.windows.push_back(tmp);
}

void checkWindowEvents(WindowHandler & windowHandler) {
	glfwWaitEvents();
	for (unsigned int i = 0; i < windowHandler.windows.size(); i++) {
		if (windowHandler.windows[i]->duplicateWindow) {
			createNewWindow(windowHandler, *windowHandler.windows[i]->renderingSystem->gameServer); //sketchy
			windowHandler.windows[i]->duplicateWindow = false;
		}
		if (glfwWindowShouldClose(windowHandler.windows[i]->glfwWindow)) {
			windowHandler.windows.erase(windowHandler.windows.begin() + i);
		}
	}
}

WindowHandler::WindowHandler() {
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
	debugPrint("|-WindowHandler was created!");
}
WindowHandler::~WindowHandler() {
	windows.clear();
	glfwTerminate();
	debugPrint("|-WindowHandler was destroyed!");
}