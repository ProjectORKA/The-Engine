#pragma once

#include "Game.hpp"

struct Window;

struct AIPlayground : public Game {
	using Game::Game;

	void render(Window& window) override;
};