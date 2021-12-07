#pragma once

#include "Game.hpp"
#include "WindowAPI.hpp"

struct Action {
	Bool pressed = false;
};

struct Mooncrash : public Game {
	Action	forward;
	Action	backward;
	Action	left;
	Action	right;
	Action	upward;
	Action	downward;
	Action	jump;
	Action	wireframe;

	Float mouseSensitivity = 0.0015f;

	PlanetCamera camera;

	Time time;
	PlanetSystem planetSystem;
	PlanetRenderSystem planetRenderSystem;

	void update() override;
	void render(Renderer & renderer) override;
	
	//input
	void mouseIsMoving(Window& window, IVec2 position) override;
	void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) override;
	void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) override;;

};