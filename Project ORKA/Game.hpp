#pragma once

#include "UserInterface.hpp"

struct GameSimulation
{
	virtual void        stop() final;
	virtual void        destroy() = 0;
	[[nodiscard]] Bool  isLoaded() const;
	virtual             ~GameSimulation();
	[[nodiscard]] Float getTickRate() const;
	[[nodiscard]] Bool  getKeepRunning() const;
	virtual void        update(Float delta) = 0;
	virtual void        create(ResourceManager& resourceManager) = 0;
	virtual void        start(ResourceManager& resourceManager) final;
private:
	Thread thread;
	Float  tickRate    = 60;
	Bool   loaded      = false;
	Bool   keepRunning = false;
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : UIElement {};
