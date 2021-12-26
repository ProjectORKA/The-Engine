
#pragma once

#include "Basics.hpp"

extern Index newUIelementID;

struct Renderer;
struct Window;

struct UIElement {
	Index id = newUIelementID++;

	virtual void render(Renderer& renderer) {};
	virtual void mouseIsMoving(Window& window, IVec2 position) {};
	virtual void filesDropped(Window& window, Vector<Path> paths) {};
	virtual void mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {};
	virtual void buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {};

	~UIElement();
};