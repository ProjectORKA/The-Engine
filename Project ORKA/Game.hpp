
#pragma once

#include "Renderer.hpp"
#include "UIElement.hpp"

struct GameSimulation{
	Bool loaded = false;
	virtual void create() {};
	virtual void destroy() {};
	virtual void update(Float delta) {};
};

struct GameRenderer : public UIElement{
	virtual void destroy() {};
	virtual void create(Window & window) {};
};