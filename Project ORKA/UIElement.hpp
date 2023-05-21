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
	U16 width = -1;
	U16 height = -1;

	void update(TiledRectangle& area) const;
};

struct UiElement
{
	virtual ~UiElement();
	Constraints constraints;
	Index id = nextInteractiveElementId++;

	UiElement& padding(U16 width);

	virtual void update(Window& window);
	virtual void destroy(Window& window);
	virtual void inputEvent(Window& window, InputEvent input);
	virtual void create(ResourceManager& resourceManager, Window& window);
	virtual void render(ResourceManager& resourceManager, Window& window, TiledRectangle area);
	virtual void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area);
};