#pragma once

#include "RenderingSystem.hpp"//always before "WindowHandler.hpp"
#include "WindowHandler.hpp"
#include "GameWorld.hpp"

class Program {
public:
	Program() {
		GameWorld gameWorld;
		RenderingSystem renderingSystem(gameWorld);
		WindowHandler windowHandler(renderingSystem);
		//Update Loop
		while (windowHandler.windows.size() > 0) {
			windowHandler.update();
			windowHandler.render();
		}
	}
};