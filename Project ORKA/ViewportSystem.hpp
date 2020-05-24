#pragma once

#include "Framebuffer.hpp"
#include "GraphicsAPI.hpp"
#include "BasicsModule.hpp"

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

	void update(Framebuffer& framebuffer);
	float aspectRatio();
	void render();
};

//[TODO] turn into RegionSystem using glScissor instead of viewport;
struct ViewportSystem {
	Vector<Viewport> viewports;
	Map<String, Index> viewportNames;
	Index currentViewport = 0;

	void create();
	void add(String name, Float x, Float y, Float w, Float h);
	void select(String name);
	void render(Framebuffer & framebuffer);
	Viewport& current();
	void destroy();
};