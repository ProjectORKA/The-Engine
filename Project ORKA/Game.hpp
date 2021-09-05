
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"
#include "GameSimulation.hpp"

struct Window;

struct Game {
	GameSimulation gameSimulation;

	virtual void update() {};
	virtual void render(Window & window) {};
	virtual void destroy() {};

	Game(Window& window); //the game will inject itself into the window
};