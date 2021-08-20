
#include "GameSimulation.hpp"

GameSimulation gameSimulation;

GameSimulation::GameSimulation()
{
	planetSystem.create();
	thread.start(GameSimulationThread, *this);
}
GameSimulation::~GameSimulation() {
	thread.stop();
	planetSystem.destroy();
}

void GameSimulation::update() {
	gameTime.update();

	planetSystem.update();
}

void GameSimulationThread(GameSimulation& gameSimulation) {
	gameSimulation.gameTime.reset();

	TimePoint t;

	while (gameSimulation.thread.keepThreadRunning) {

		t = Clock::now() + Milliseconds(16);

		gameSimulation.update();

		sleepUntil(t);
	}
}