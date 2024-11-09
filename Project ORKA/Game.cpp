#include "Game.hpp"

void GameSimulation::stop()
{
	if (loaded)
	{
		logDebug("Stopping game simulation thread!");
		keepRunning = false;
		destroy();
		loaded = false;
		thread.join();
	}
	else
	{
		logError("Simulation not initialized!");
	}
}

void GameSimulation::start()
{
	if (!loaded)
	{
		create();
		loaded      = true;
		keepRunning = true;
		logDebug("Starting game simulation thread!");
		thread = Thread(gameSimulationThread, std::ref(*this));
	}
	else
	{
		logError("Simulation already initialized!");
	}
}

Bool GameSimulation::isLoaded() const
{
	return loaded;
}

Float GameSimulation::getTickRate() const
{
	return tickRate;
}

Bool GameSimulation::getKeepRunning() const
{
	return keepRunning;
}

void gameSimulationThread(GameSimulation& sim)
{
	if (sim.isLoaded())
	{
		logDebug("Game simulation started!");
		while (sim.getKeepRunning())
		{
			TimePoint t = Clock::now() + Milliseconds(static_cast<Int>(1000.0f / sim.getTickRate()));
			sim.update(1.0f / sim.getTickRate());
			sleepUntil(t);
		}
		logDebug("Game simulation stopped!");
	}
	else
	{
		logError("Simulation not initialized!");
	}
}
