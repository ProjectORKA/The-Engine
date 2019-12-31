
#include "Program.hpp"

void startGameSimulation(GameSimulation & gameSimulation) {

	gameSimulation.gameTime.resetTime();

	chunkIsInUse(gameSimulation.worldSystem.root);

	generateEntities(gameSimulation.worldSystem.root, gameSimulation);

	gameSimulation.thread = std::make_unique<std::thread>(GameSimulationThread, std::ref(gameSimulation));
};

void stopGameSimulation(GameSimulation & gameSimulation) {
	gameSimulation.keepThreadRunning = false;
	gameSimulation.thread->join();
}

void GameSimulationThread(GameSimulation & gameSimulation) {

	gameSimulation.gameTime.paused = true;								//<-- starts game paused [TODO] switch on

	std::chrono::steady_clock::time_point t;

	while (gameSimulation.keepThreadRunning) {

		t = std::chrono::steady_clock::now();
		t += std::chrono::milliseconds(8); //game simulation running at 120 Hz
		///////////////////////////////////////////////////////////////
		gameSimulation.gameTime.processTime();
		processWorldSystem(gameSimulation.worldSystem.root, gameSimulation);
		processModelMatrixSystem(gameSimulation);
		processTransformationSystem(gameSimulation);

		///////////////////////////////////////////////////////////////
		std::this_thread::sleep_until(t);

	}
}

void processWorldSystem(Chunk& chunk, GameSimulation& gameSimulation) {
	if (((chunk.expirationDate - std::chrono::steady_clock::now()).count() > 0) | (chunk.level == 0)) {
		if (chunk.subdivided) {
			processWorldSystem(*chunk.tfr, gameSimulation);
			processWorldSystem(*chunk.tfl, gameSimulation);
			processWorldSystem(*chunk.tbr, gameSimulation);
			processWorldSystem(*chunk.tbl, gameSimulation);
			processWorldSystem(*chunk.bfr, gameSimulation);
			processWorldSystem(*chunk.bfl, gameSimulation);
			processWorldSystem(*chunk.bbr, gameSimulation);
			processWorldSystem(*chunk.bbl, gameSimulation);
		}
		else {
			subdivideChunk(chunk, gameSimulation);
		}
	}
	else {
		if (chunk.subdivided) {
			unsubdivideChunk(chunk, gameSimulation);
		}
	}
}

void processModelMatrixSystem(GameSimulation& gameSimulation)
{
	gameSimulation.transformationSystem.mutex.lock_shared();
	gameSimulation.modelMatrixSystem.mutex.lock();
	
	for (Index modelMatrixIndex = 0; modelMatrixIndex < gameSimulation.modelMatrixSystem.modelMatrices.size(); modelMatrixIndex++) {
		gameSimulation.modelMatrixSystem.modelMatrices[modelMatrixIndex] = gameSimulation.transformationSystem.transformations[modelMatrixIndex].calculateModelMatrix();
	}

	gameSimulation.modelMatrixSystem.mutex.unlock();
	gameSimulation.transformationSystem.mutex.unlock_shared();
}

void processTransformationSystem(GameSimulation& gameSimulation)
{
	gameSimulation.transformationSystem.mutex.lock();
	for (int i = 0; i < gameSimulation.transformationSystem.transformations.size(); i++) {
		gameSimulation.transformationSystem.transformations[i].setRotation(glm::vec3(0,0,gameSimulation.gameTime.total()));
	}
	gameSimulation.transformationSystem.mutex.unlock();
}
