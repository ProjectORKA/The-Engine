
#pragma once

#include "UserInterface.hpp"

struct GameSimulation {
	Thread thread;
	Bool loaded = false;
	Float tickrate = 60;
	Bool keepRunning = false;

	virtual void stop();
	virtual void start(ResourceManager& resourceManager);
	virtual void create(ResourceManager& resourceManager) {};
	virtual void destroy() {};
	virtual void update(Float delta) {};
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : public UIElement {
	GameSimulation simulation;
};