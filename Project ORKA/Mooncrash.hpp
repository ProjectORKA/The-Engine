#pragma once

#include "Game.hpp"
#include "WindowSystem.hpp"

struct Action {
	Bool pressed = false;
};

struct Mooncrash : public Game {
	using Game::Game;//workaround to use the base constructor

	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;

	PlanetCamera camera;

	void update() override;
	void render(Window& window) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
};