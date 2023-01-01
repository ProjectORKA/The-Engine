
#include "Game.hpp"

void gameSimulationThread(GameSimulation& sim) {
	TimePoint t;
	logDebug("Game simulation started!");
	while (sim.keepRunning) {
		t = Clock::now() + Milliseconds(Int(1000.0f / sim.tickrate));
		sim.update(1.0f / sim.tickrate);
		sleepUntil(t);
	}
	logDebug("Game simulation stopped!");
}

void GameSimulation::init(Engine& engine) {
	logDebug("Starting game simulation thread!");
	keepRunning = true;
	thread.start(gameSimulationThread, *this);
}

void GameSimulation::destroy() {
	logDebug("Stopping game simulation thread!");
	keepRunning = false;
	thread.stop();
}
