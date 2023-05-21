#include "Game.hpp"

void gameSimulationThread(GameSimulation& sim) {
	TimePoint t;
	logDebug("Game simulation started!");
	while (sim.keepRunning) {
		t = Clock::now() + Milliseconds(static_cast<Int>(1000.0f / sim.tickRate));
		sim.update(1.0f / sim.tickRate);
		sleepUntil(t);
	}
	logDebug("Game simulation stopped!");
}

void GameSimulation::start(ResourceManager& resourceManager) {
	logDebug("Starting game simulation thread!");
	keepRunning = true;
	thread = Thread(gameSimulationThread, std::ref(*this));
}

void GameSimulation::stop() {
	logDebug("Stopping game simulation thread!");
	keepRunning = false;
	thread.join();
}
