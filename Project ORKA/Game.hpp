
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "GameSimulation.hpp"

struct Game {
	GameSimulation gameSimulation;

	virtual void update() {};
	virtual void destroy() {};
	virtual void action(String action) {};
	virtual void render(Window & window) {};
	virtual void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {};

	Game(Window& window); //the game will inject itself into the window
};