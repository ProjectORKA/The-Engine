
#include "Program.hpp"

void Program::run()
{
	windowSystem.addWindow();
	windowSystem.processLoop();
}

Program program;