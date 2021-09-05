
#pragma once

#include "Time.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "Threading.hpp"
#include "PlanetSystem.hpp"
#include "Renderer.hpp"

//the GameSimulation simulates complex non-visual parts of a Game
//it starts its own thread upon creation
//it can be used without a renderer, to act as a server

struct GameSimulation {
	Time time;
	PlanetSystem planetSystem;

	Thread thread;

	GameSimulation();
	~GameSimulation();
	void update();
};

void GameSimulationThread(GameSimulation& gameSimulation);