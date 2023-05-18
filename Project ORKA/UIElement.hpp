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

	void update(TiledRectangle& area) const {
		if (paddingX != static_cast<U16>(-1)) {
			area.size.x -= 2 * paddingX;
			area.position.x += paddingX;
		}
		if (paddingY != static_cast<U16>(-1)) {
			area.size.y -= 2 * paddingY;
			area.position.y += paddingY;
		}
		if (width != static_cast<U16>(-1)) area.size.x = width;
		if (height != static_cast<U16>(-1)) area.size.y = height;
	}
};

struct UIElement {
	virtual ~UIElement() = default;
	Constraints constraints;
	Index id = nextInteractiveElementID++;

	UIElement& padding(U16 width);

	virtual void update(Window& window) {};
	virtual void destroy(Window& window) {};
	virtual void create(ResourceManager& resourceManager, Window& window) {};
	virtual void inputEvent(Window& window, InputEvent input) {};
	virtual void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {};

	virtual void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
		OPTICK_EVENT();
	};
};
