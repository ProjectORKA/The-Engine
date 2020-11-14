
#include "GameSimulation.hpp"

void GameSimulation::start()
{
	keepThreadRunning = true;
	thread = Thread(GameSimulationThread, std::ref(*this));
}

void GameSimulation::stop() {
	keepThreadRunning = false;
	thread.join();
}

void GameSimulation::update() {

	planetSystem.update();

}

void GameSimulationThread(GameSimulation& gameSimulation) {

	gameSimulation.gameTime.reset();

	TimePoint t;

	while (gameSimulation.keepThreadRunning) {

		t = std::chrono::steady_clock::now() + std::chrono::milliseconds(16); //game simulation running at 60 Hz

		gameSimulation.gameTime.update();

		gameSimulation.update();

		//wait for next tick
		std::this_thread::sleep_until(t);
	}
}