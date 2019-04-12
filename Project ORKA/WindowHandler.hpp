#pragma once
#ifndef WINDOWHANDLER_HPP
#define WINDOWHANDLER_HPP


#include <iostream>
#include <vector>
#include "RenderingSystem.hpp"
#include "GLFW/glfw3.h"
#include "Debug.hpp"
#include "Window.hpp"

//predefenition for callbacks
void whenGLFWThrowsError(int error, const char* description);

class WindowHandler {
public:
	RenderingSystem & renderingSystem;
	std::vector<Window *> windows;
	WindowHandler(RenderingSystem & renderingSystem) : renderingSystem(renderingSystem) {
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

		addWindow();
		addWindow();
	}
	~WindowHandler() {
		glfwTerminate();
	}
	void addWindow() {
		debugPrint("Adding Window...");
		Window * tmp = new Window;
		windows.push_back(tmp);
	}
	void update() {
		glfwPollEvents();
		for (int i = 0; i < windows.size(); i++) {
			if (glfwWindowShouldClose(windows[i]->glfwWindow)) {
				delete windows[i];
				windows.erase(windows.begin() + i);
			}
		}
	};
	void render() {
		for (int i = 0; i < windows.size(); i++) {
			windows[i]->render(renderingSystem);
		}

	}
};

//callbacks
void whenGLFWThrowsError(int error, const char* description)
{
	std::cout << "Error: " << description << std::endl;
	std::getchar();
	exit(EXIT_FAILURE);
}


#endif // !WINDOWHANDLER_HPP