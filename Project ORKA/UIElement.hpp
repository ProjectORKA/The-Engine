
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "TiledMath.hpp"
#include "Renderer.hpp"
#include "Input.hpp"
#include "Profiler.hpp"

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
	Constraints constraints;
	Index id = nextInteractiveElementID++;

	~UIElement();
	UIElement& padding(U16 width);

	virtual void update(Window& window) {};
	virtual void destroy(Window& window) {};
	virtual void create(Engine& engine, Window& window) {};
	virtual void inputEvent(Window& window, InputEvent input) {};
	virtual void render(Engine& engine, Window& window, TiledRectangle area) {};
	virtual void renderInteractive(Engine& engine, Window& window, TiledRectangle area) { OPTICK_EVENT(); };
};