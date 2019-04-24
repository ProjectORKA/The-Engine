#pragma once
#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <iostream>
#include "RenderingSystem.hpp"
#include "Input.hpp"
#include "Debug.hpp"
#include "GLFW/glfw3.h"

void whenWindowIsMoved(GLFWwindow* window, int xPos, int yPos);
void whenWindowIsResized(GLFWwindow* window, int width, int height);
void whenWindowChangedFocus(GLFWwindow* window, int focused);
void whenMouseIsScrolling(GLFWwindow* window, double xoffset, double yoffset);
void whenMouseIsPressed(GLFWwindow* window, int button, int action, int mods);
void whenButtonIsPressed(GLFWwindow* window, int key, int scancode, int action, int mods);
void APIENTRY DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

class Window {
public:
	unsigned int antiAliasing;
	bool capturingCursor;
	//settings
	std::string title;
	bool fullScreen;
	unsigned int width;
	unsigned int height;
	int winPosX, winPosY;
	double curPosX, curPosY;
	double deltaX, deltaY;
	GLFWwindow * glfwWindow;
	RenderingSystem * renderingSystem;
	InputHandler input;

	Window(RenderingSystem * renderingSystem);
	void createTheWindow();
	void destroyTheWindow();
	void reloadTheWindow();
	void render();
	void inputs();
	void changeAntiAliasing(unsigned int antiAliasing);
	void captureCursor();
	void uncaptureCursor();
	void setWindowHints();
	void centerWindow();
	void toggleFullscreen();
	void setIcon(std::string path);
	void setWindowCallbacks();
	~Window();
};
#endif // !WINDOW_HPP
