#include "GameSystem.hpp"
#include "Random.hpp"

GameSystem* gameSystem = nullptr;

void GameSystem::run() {
	//creates the gamesimulation thread which continually updates all games
	thread.start(gameSimulationThread, thread, *this);
}
void GameSystem::stop() {
	//stops the game simulation thread, meaning all games wont be updated anymore
	//also deletes the game simulations
	
	randomizeSeed();
	
	thread.stop();
	for (GameSimulation* gameSimulation : games) {
		gameSimulation->destroy();
	}
	games.clear();
}
void GameSystem::add(GameSimulation & game) {
	game.create();
	games.push_back(&game);
}
void gameSimulationThread(Thread& thread, GameSystem & gameSystem) {

	TimePoint t;

	while (thread.keepThreadRunning) {

		Float framerate = 144;

		t = Clock::now() + Milliseconds(Int(1000.0f / framerate));

		for (GameSimulation* gameSimulation : gameSystem.games) {
			gameSimulation->update(1/framerate);
		}

		sleepUntil(t);
	}
}

void initializeGameSystem() {
	gameSystem = new GameSystem();
}

void destroyGameSystem() {
	delete gameSystem;
}
