
#include "Program.hpp"

void Program::start()
{
	gameSimulation.start();

	windowSystem.start();
	
	windowSystem.addWindow(gameSimulation);

	windowSystem.processLoop(gameSimulation);

	windowSystem.stop();

	gameSimulation.stop();
}