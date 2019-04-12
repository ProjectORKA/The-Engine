#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "RenderingSystem.hpp"
#include "Debug.hpp"
#include "GLFW/glfw3.h"

class Window {
public:
	GLFWwindow * glfwWindow;

	Window() {
		try {
			glfwWindow = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
			if (!glfwWindow) {
				throw std::exception("Failed to initialize Window!");
			}
		}
		catch (std::exception error) {
			std::cout << "Error: " << error.what() << std::endl;
			std::getchar();
			exit(EXIT_FAILURE);
		};
		glfwMakeContextCurrent(glfwWindow);

		debugPrint("Initializing GLEW...");
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


	}
	void render(RenderingSystem & renderingSystem) {
		glfwMakeContextCurrent(glfwWindow);
		renderingSystem.render();
		glfwSwapBuffers(glfwWindow);
	}
	~Window() {
		glfwDestroyWindow(glfwWindow);
	}
};
#endif // !WINDOW_HPP
