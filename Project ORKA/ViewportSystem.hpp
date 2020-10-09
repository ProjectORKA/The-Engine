#pragma once

#include "Framebuffer.hpp"
#include "GraphicsAPI.hpp"
#include "Basics.hpp"

struct Viewport {
	Float relativeX = 0.0f;
	Float relativeY = 0.0f;
	Float relativeWidth = 1.0f;
	Float relativeHeight = 1.0f;

	//needs update
	Float absoluteX = 0.0f;
	Float absoluteY = 0.0f;
	Float absoluteWidth = 0.0f;
	Float absoluteHeight = 0.0f;

	void update(Int & adaptiveWidth, Int & adaptiveHeight);
	float aspectRatio();
	void render();
};

//[TODO] turn into RegionSystem using glScissor instead of viewport;
struct ViewportSystem {
	void create();
	void destroy();
	Viewport& current();
	void select(String name);
	void render(Int adaptiveWidth , Int adaptiveHeight);
	void add(String name, Float x, Float y, Float w, Float h);

private:
	Vector<Viewport> viewports;
	Map<String, Index> viewportNames;
	Index currentViewport = 0;
};