
#include "Program.hpp"

void stopGameSimulation(GameSimulation & gameSimulation) {
	gameSimulation.keepThreadRunning = false;
	gameSimulation.thread->join();
}

void startGameSimulation(GameSimulation & gameSimulation) {

	gameSimulation.gameTime.resetTime();

	chunkIsInUse(gameSimulation.worldSystem.octreeRoot);

	generateEntities(gameSimulation.worldSystem.octreeRoot, gameSimulation);

	gameSimulation.thread = std::make_unique<std::thread>(GameSimulationThread, std::ref(gameSimulation));
};

int GameSimulationThread(GameSimulation & gameSimulation) {

	std::chrono::steady_clock::time_point t;

	while (gameSimulation.keepThreadRunning) {

		t = std::chrono::steady_clock::now();
		t += std::chrono::milliseconds(8); //game simulation running at 120 Hz
		///////////////////////////////////////////////////////////////
		gameSimulation.gameTime.processTime();
		processWorldSystem(gameSimulation.worldSystem.octreeRoot, gameSimulation);
		processModelMatrixSystem(gameSimulation);
		processTransformationSystem(gameSimulation);

		///////////////////////////////////////////////////////////////
		std::this_thread::sleep_until(t);

	}

	return 0;
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

void processWorldSystem(std::shared_ptr<Chunk> chunk, GameSimulation& gameSimulation) {
	Chunk& rChunk = *chunk;
	if (((rChunk.expirationDate - std::chrono::steady_clock::now()).count() > 0) | (rChunk.level == 0)) {
		if (rChunk.subdivided) {
			processWorldSystem(rChunk.tfr, gameSimulation);
			processWorldSystem(rChunk.tfl, gameSimulation);
			processWorldSystem(rChunk.tbr, gameSimulation);
			processWorldSystem(rChunk.tbl, gameSimulation);
			processWorldSystem(rChunk.bfr, gameSimulation);
			processWorldSystem(rChunk.bfl, gameSimulation);
			processWorldSystem(rChunk.bbr, gameSimulation);
			processWorldSystem(rChunk.bbl, gameSimulation);
		}
		else {
			subdivideChunk(chunk, gameSimulation);
		}
	}
	else {
		if (rChunk.subdivided) {
			unsubdivideChunk(chunk, gameSimulation);
		}
	}
}
