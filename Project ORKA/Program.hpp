# pragma once

#include "GameSimulation.hpp"
#include "WindowSystem.hpp"

struct Program {
	GameSimulation gameSimulation;
	WindowSystem windowSystem;

	void start();
};