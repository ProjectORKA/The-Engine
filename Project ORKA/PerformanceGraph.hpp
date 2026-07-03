#pragma once

#include "Basics.hpp"
#include "Window.hpp"

struct PerformanceGraph final : UIElement
{
	Int           width     = 500;
	Int           height    = 200;
	Float         maxSample = 0.0f;
	Vector<Float> times     = Vector<Float>(width);

	void addTime(Float time);
	void render(Window& window) override;
	void update(Window& window) override {}
	void create(Window& window) override {}
	void destroy(Window& window) override {}
	void renderInteractive(Window& window) override {}
	void inputEvent(Window& window, InputEvent& input) override {}
};
