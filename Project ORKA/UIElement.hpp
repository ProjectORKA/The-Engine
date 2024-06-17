#pragma once

#include "Basics.hpp"
#include "TiledMath.hpp"
#include "Renderer.hpp" //do not remove
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
	Bool        hasBackground   = false;
	String      debugName            = "Unknown";
	Color       backgroundColor = Color(0, 0, 0, 0);
	Index       id              = nextInteractiveElementId++;

	virtual      ~UIElement();
	virtual void update(Window& window) = 0;
	virtual void create(Window& window) = 0;
	virtual void destroy(Window& window) = 0;
	virtual void render(Window& window, TiledRectangle area) = 0;
	virtual void inputEvent(Window& window, InputEvent input) = 0;
	virtual void renderInteractive(Window& window, TiledRectangle area) = 0;

	UIElement& fill(Color color);
	UIElement& padding(Int width);
	UIElement& name(const String& debugName);
};
