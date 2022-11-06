
#pragma once

#include "UserInterface.hpp"

struct GameSimulation {
	Thread thread;
	Bool loaded = false;
	Float tickrate = 60;
	Bool keepRunning = false;

	virtual void init();
	virtual void destroy();
	virtual void update(Float delta) {};
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : public UIElement {
};