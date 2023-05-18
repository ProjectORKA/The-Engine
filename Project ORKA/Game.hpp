#pragma once

#include "UserInterface.hpp"

struct GameSimulation {
	virtual ~GameSimulation() = default;
	Thread thread;
	Bool loaded = false;
	Float tickRate = 60;
	Bool keepRunning = false;

	virtual void stop();
	virtual void start(ResourceManager& resourceManager);
	virtual void create(ResourceManager& resourceManager) {};
	virtual void destroy() {};
	virtual void update(Float delta) {};

	GameSimulation() = default;

	bool operator==(const GameSimulation& other) const = delete;
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : public UIElement {
	GameSimulation simulation;

	GameRenderer() = default;

	bool operator==(const GameRenderer& other) const = default;
};
