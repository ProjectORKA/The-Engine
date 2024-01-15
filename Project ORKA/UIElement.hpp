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
	Int paddingX = 0;
	Int paddingY = 0;
	Int width    = 0;
	Int height   = 0;

	void update(TiledRectangle& area) const;
};

struct UIElement
{
	Constraints constraints;
	Index       id = nextInteractiveElementId++;

	virtual      ~UIElement();
	UIElement&   padding(Int width);
	virtual void update(Window& window) = 0;
	virtual void create(Window& window) = 0;
	virtual void destroy(Window& window) = 0;
	virtual void render(Window& window, TiledRectangle area) = 0;
	virtual void inputEvent(Window& window, InputEvent input) = 0;
	virtual void renderInteractive(Window& window, TiledRectangle area) = 0;
};
