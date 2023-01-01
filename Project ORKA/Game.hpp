
#pragma once

#include "UserInterface.hpp"

struct GameSimulation {
	Thread thread;
	Bool loaded = false;
	Float tickrate = 60;
	Bool keepRunning = false;

	virtual void destroy();
	virtual void init(Engine& engine);
	virtual void update(Float delta) {};
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : public UIElement {
};