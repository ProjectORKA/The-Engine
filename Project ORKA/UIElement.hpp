
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "TiledMath.hpp"
#include "Renderer.hpp"
#include "KeyMap.hpp"

extern Index newUIElementID;

struct Renderer;
struct Window;

struct UIElement {
	Index id = newUIElementID++;

	~UIElement();
	virtual void update(Renderer& renderer) {};
	virtual void mouseIsMoving(Window& window, IVec2 position) {};
	virtual void filesDropped(Window& window, Vector<Path> paths) {};
	virtual void render(TiledRectangle renderArea, Renderer& renderer) {};
	virtual void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {};
	virtual void renderInteractive(TiledRectangle renderArea, Renderer& renderer) {};
	virtual void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {};
	virtual void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {};
};
