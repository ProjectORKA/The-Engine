
#pragma once

#include "Time.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "PlanetSystem.hpp"
//the GameSimulation simulates the non visual parts of the game world and acts as a server
//gamesimulation needs to start before any windows/renderers/users/players can access it
//gamesimulation needs to stop after all players have disconnected
struct GameSimulation {
	Time gameTime;

	Vector<SpaceShip> spaceShips;

	PlanetSystem planetSystem;

	//WorldChunk world;

	//thread
	Bool keepThreadRunning = true;
	Thread thread;

	void start();
	void update();
	void stop();
};

void GameSimulationThread(GameSimulation& gameSimulation);
