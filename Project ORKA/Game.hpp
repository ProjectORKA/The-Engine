#pragma once

#include "UserInterface.hpp"

struct GameSimulation
{
	virtual void        stop() final;
	virtual void        destroy() = 0;
	[[nodiscard]] Bool  isLoaded() const;
	[[nodiscard]] Float getTickRate() const;
	[[nodiscard]] Bool  getKeepRunning() const;
	virtual void        update(Float delta) = 0;
	virtual void        create() = 0;
	virtual void        start() final;
private:
	Thread thread;
	Float  tickRate    = 60;
	Bool   loaded      = false;
	Bool   keepRunning = false;
};

void gameSimulationThread(GameSimulation& sim);

struct GameRenderer : UIContainer
{
	virtual void connect(GameSimulation& simulation) = 0;
};
