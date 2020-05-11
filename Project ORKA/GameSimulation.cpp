
#include "GameSimulation.hpp"

void GameSimulation::process() {
	processSubdivision(world, *this);
}

void GameSimulationThread(GameSimulation & gameSimulation) {

	gameSimulation.gameTime.reset();

	TimePoint t;

	while (gameSimulation.keepThreadRunning) {

		t = std::chrono::steady_clock::now() + std::chrono::milliseconds(8); //game simulation running at 120 Hz

		gameSimulation.gameTime.update();

		gameSimulation.process();

		//wait for next tick
		std::this_thread::sleep_until(t);
	}
	
	//cleanup
	//unsubdivideChunk(*gameSimulation.worldSystem.octreeRoot);
	//destroyEntities(*gameSimulation.worldSystem.octreeRoot);
}

void GameSimulation::start()
{
	keepThreadRunning = true;
	thread = Thread(GameSimulationThread, std::ref(*this));
}

void GameSimulation::stop() {
	keepThreadRunning = false;
	thread.join();
}
