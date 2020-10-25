
#pragma once

#include "Time.hpp"
#include "Chunk.hpp"
#include "Math.hpp"
#include "Entity.hpp"
#include "PlanetSystem.hpp"

struct GameSimulation {
	Time gameTime;

	Vector<SpaceShip> spaceShips;

	PlanetSystem planetSystem;

	//WorldChunk world;

	//thread
	Bool keepThreadRunning = true;
	Thread thread;

	void start();
	void process();
	void stop();
};

void GameSimulationThread(GameSimulation& gameSimulation);
