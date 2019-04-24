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
	std::vector<Window *> windows;
	WindowHandler();
	~WindowHandler();
	void addWindow(RenderingSystem * renderingSystem);
	void update();
	void render();
};
#endif // !WINDOWHANDLER_HPP