#pragma once

#include "Game.hpp"

#include "Pong.hpp"
#include "Intro.hpp"
#include "Mooncrash.hpp"
#include "AIPlayground.hpp"

struct GameSystem {
	List<Game *> games;

	void add(Game* game);
	void remove(Index id);

	~GameSystem();
};

extern GameSystem gameSystem;