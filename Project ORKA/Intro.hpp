#pragma once

#include "Game.hpp"
#include "Mooncrash.hpp"

struct Intro : public Game {
	using Game::Game;

	void update() override;
	void render(Window & window) override;
};