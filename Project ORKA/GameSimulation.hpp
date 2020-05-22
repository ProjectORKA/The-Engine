
#pragma once

#include "Time.hpp"
#include "Chunk.hpp"
#include "Math.hpp"
#include "Entity.hpp"




struct GameSimulation {
	Time gameTime;

	Vector<SpaceShip> spaceShips;

	Chunk world;

	//thread
	Bool keepThreadRunning = true;
	Thread thread;

	void start();
	void process();
	void stop();
};

void GameSimulationThread(GameSimulation& gameSimulation);
