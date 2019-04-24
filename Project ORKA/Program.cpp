
#include "Program.hpp"

Program::Program() {
	GameServer gameWorld;
	RenderingSystem renderingSystem(gameWorld);
	WindowHandler windowHandler;
	windowHandler.addWindow(&renderingSystem);
	//Update Loop
	while (windowHandler.windows.size() > 0) {
		windowHandler.update();

		windowHandler.render();
	}
};