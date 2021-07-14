
#pragma once

#include "Window.hpp"

struct WindowSystem {
	
	List<Window> windows;

	WindowSystem();
	~WindowSystem();
	void startMainLoop();
	Window & addWindow();
};

void whenWindowAPIThrowsError(Int error, const char* description);

extern WindowSystem windowSystem;