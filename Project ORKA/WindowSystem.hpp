
#pragma once

#include "Window.hpp"

struct WindowSystem {
	
	List<Window> windows;

	WindowSystem();
	~WindowSystem();
	void startMainLoop();
	Window & addWindow();
};

void whenWindowAPIThrowsError(Int error, const Char* description);

extern WindowSystem windowSystem;