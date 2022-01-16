#include "GameSystem.hpp"

GameSystem gameSystem;

GameSystem::~GameSystem() {
	//thread.stop();
	for (Game* game : games) {
		game->destroy();
		delete game;
	}
	games.clear();
}
Game * GameSystem::add(Game* game) {
	games.push_back(game);
	return games.back();
}
void GameSystem::run() {
	thread.start(gameSimulationThread, thread, *this);
}
void gameSimulationThread(Thread& thread, GameSystem & gameSystem) {

	TimePoint t;

	while (thread.keepThreadRunning) {

		t = Clock::now() + Milliseconds(Int(1.0f / 60.0f));

		for (Game* game : gameSystem.games) {
			game->update();
		}

		sleepUntil(t);
	}
}
