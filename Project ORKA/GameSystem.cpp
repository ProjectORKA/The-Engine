#include "GameSystem.hpp"

GameSystem gameSystem;

void GameSystem::add(Game* game) {
	games.push_back(game);
}

GameSystem::~GameSystem() {
	for (Game* game : games) {
		game->destroy();
		delete game;
	}
	games.clear();
}
