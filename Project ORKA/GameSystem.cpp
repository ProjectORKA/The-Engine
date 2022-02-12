#include "GameSystem.hpp"
#include "Random.hpp"

GameSystem gameSystem;

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

		t = Clock::now() + Milliseconds(Int(1000.0f / 144.0f));

		for (GameSimulation* gameSimulation : gameSystem.games) {
			gameSimulation->update();
		}

		sleepUntil(t);
	}
}