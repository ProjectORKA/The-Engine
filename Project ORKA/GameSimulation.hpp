
#pragma once

#include "Time.hpp"
#include "Chunk.hpp"

struct GameSimulation {
	Time gameTime;

	Chunk world;

	//thread
	Bool keepThreadRunning = true;
	Thread thread;

	void start();
	void process();
	void stop();
};

void GameSimulationThread(GameSimulation& gameSimulation);