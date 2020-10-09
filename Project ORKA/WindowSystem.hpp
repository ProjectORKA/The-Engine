
#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct WindowSystem {
	List<Window> windows;

	void start();
	void addWindow(GameSimulation & gameSimulation);
	void processLoop(GameSimulation& gameSimualtion);
	void stop();
};

void whenWindowAPIThrowsError(Int error, const char* description);