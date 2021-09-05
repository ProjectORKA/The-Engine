
#include "Program.hpp"
#include "GameSystem.hpp"

void Program::run()
{
	windowSystem.addWindow();
	gameSystem.add(new AIPlayground(windowSystem.windows.back()));
	windowSystem.startMainLoop();
}

Program program;