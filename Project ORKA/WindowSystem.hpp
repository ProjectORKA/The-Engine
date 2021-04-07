
#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct WindowSystem {
	
	List<Window> windows;
	
	//Mutex mutex;

	WindowSystem();
	~WindowSystem();
	void processLoop();
	void addWindow();
};

extern WindowSystem windowSystem;

void whenWindowAPIThrowsError(Int error, const char* description);