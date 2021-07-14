
#pragma once

#include "Time.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "Threading.hpp"
#include "PlanetSystem.hpp"
#include "Renderer.hpp"

//the GameSimulation simulates the non visual parts of the game world
//ideally it doesent know what and where the players are

struct GameSimulation {
	Time gameTime;
	PlanetSystem planetSystem;
	//Renderer renderer;

	Thread thread;

	GameSimulation();
	~GameSimulation();
	void update();
};

extern GameSimulation gameSimulation;

void GameSimulationThread(GameSimulation& gameSimulation);