
#pragma once

#include "Renderer.hpp"
#include "InputManager.hpp"

struct Game {
	
	Game* selfReplace = nullptr;

	virtual void create() {};
	virtual void update() {};
	virtual void destroy() {};
	virtual void render(Renderer & renderer) {};

	//input
	virtual void mouseIsMoving(Window & window, IVec2 position) {};
	virtual void filesDropped(Window& window, Vector<Path> paths) {};
	virtual void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {};
	virtual void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {};
};