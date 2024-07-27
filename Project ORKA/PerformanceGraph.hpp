#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct PerformanceGraph : UIElement
{
	Int           width     = 500;
	Int           height    = 200;
	Float         maxSample = 0.0f;
	Vector<Float> times     = Vector<Float>(width);

	void addTime(Float time);
	void update(Window& window) override {}
	void create(Window& window) override {}
	void destroy(Window& window) override {}
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override {}
	void renderInteractive(Window& window, TiledRectangle area) override {}
};
