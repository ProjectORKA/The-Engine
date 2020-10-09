
#include "GameSimulation.hpp"

void GameSimulation::start()
{
	UShort team = 0;

	float extend = 5;

	for (int i = 0; i < 10000; i++) {
		spaceShips.emplace_back();
		spaceShips.back().location = Vec3(randomFloat(-extend, extend), randomFloat(-extend, extend), randomFloat(-extend, extend));
		spaceShips.back().velocity = Vec3(randomFloat(-extend, extend), randomFloat(-extend, extend), randomFloat(-extend, extend));
		spaceShips.back().team = team;
	}

	keepThreadRunning = true;
	thread = Thread(GameSimulationThread, std::ref(*this));
}

void GameSimulation::stop() {
	keepThreadRunning = false;
	thread.join();
}

void GameSimulation::process() {

	world.processSubdivision(*this);
	
	if (!gameTime.paused) {
		for (SpaceShip& ship : spaceShips) {
			ship.update(gameTime);
		}
	}
}

void GameSimulationThread(GameSimulation& gameSimulation) {

	gameSimulation.gameTime.reset();

	TimePoint t;

	while (gameSimulation.keepThreadRunning) {

		t = std::chrono::steady_clock::now() + std::chrono::milliseconds(16); //game simulation running at 60 Hz

		gameSimulation.gameTime.update();

		gameSimulation.process();

		//wait for next tick
		std::this_thread::sleep_until(t);
	}
}