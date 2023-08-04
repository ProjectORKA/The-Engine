#pragma once

#include "Basics.hpp"
#include "TiledMath.hpp"
#include "Renderer.hpp"
#include "Input.hpp"

extern Index nextInteractiveElementId;

struct Renderer;
struct Window;

struct Constraints
{
	U16 paddingX = -1;
	U16 paddingY = -1;
	U16 width    = -1;
	U16 height   = -1;

	void update(TiledRectangle& area) const;
};

struct UIElement
{
	Constraints constraints;
	Index       id = nextInteractiveElementId++;

	virtual      ~UIElement();
	UIElement&   padding(U16 width);
	virtual void update(Window& window) = 0;
	virtual void destroy(Window& window) = 0;
	virtual void inputEvent(Window& window, InputEvent input) = 0;
	virtual void create(ResourceManager& resourceManager, Window& window) = 0;
	virtual void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) = 0;
	virtual void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) = 0;
};
