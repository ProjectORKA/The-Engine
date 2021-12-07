#pragma once

#include "Game.hpp"
#include "Mooncrash.hpp"

struct Intro : public Game {
	using Game::Game;

	Game * gameStartingAfterIntro = nullptr;

	void update() override;
	void render(Renderer & renderer) override;

	Intro(Game* gameStartingAfterIntro) {
		this->gameStartingAfterIntro = gameStartingAfterIntro;
	};
};