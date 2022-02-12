#pragma once

#include "Game.hpp"
#include "Mooncrash.hpp"

struct Intro : public GameRenderer {
	GameRenderer* gameStartingAfterIntro = nullptr;

	void render(TiledRectangle area, Renderer & renderer) override;
	Intro(GameRenderer* gameStartingAfterIntro);
};