#pragma once

#include "Game.hpp"
#include "WindowSystem.hpp"

struct Mooncrash : public Game {
	using Game::Game;//workaround to use the base constructor

	PlanetCamera camera;

	void update() override;
	void render(Window& window) override;;
};