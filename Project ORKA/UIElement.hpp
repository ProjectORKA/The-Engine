
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "TiledMath.hpp"
#include "Renderer.hpp"
#include "KeyMap.hpp"

extern Index nextInteractiveElementID;

struct Renderer;
struct Window;

struct Constraints {
	U16 paddingX = -1;
	U16 paddingY = -1;
	U16 width = -1;
	U16 height = -1;

	void update(TiledRectangle& area) {
		if (paddingX != U16(-1)) {

			area.size.x -= 2 * paddingX;
			area.position.x += paddingX;
		}
		if (paddingY != U16(-1)) {
			area.size.y -= 2 * paddingY;
			area.position.y += paddingY;
		}
		if (width != U16(-1)) area.size.x = width;
		if (height != U16(-1)) area.size.y = height;
	}
};

struct UIElement {
	Index id = nextInteractiveElementID++;
	Constraints constraints;

	~UIElement();
	UIElement& padding(U16 width) {
		constraints.paddingX = width;
		constraints.paddingY = width;
		return *this;
	}

	virtual void update(Renderer& renderer) {};
	virtual void mouseIsMoving(Window& window, IVec2 position) {};
	virtual void render(TiledRectangle area, Renderer& renderer) {};
	virtual void filesDropped(Window& window, Vector<Path> paths) {};
	virtual void renderInteractive(TiledRectangle area, Renderer& renderer) {};
	virtual void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {};
	virtual void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {};
	virtual void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {};
};