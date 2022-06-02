#pragma once

#include "Game.hpp"
#include "Mooncrash.hpp"

struct Intro : public GameRenderer {
	GameRenderer* gameStartingAfterIntro = nullptr;

	Intro(GameRenderer* gameStartingAfterIntro);
	void render(Window& window, TiledRectangle area) override;
};