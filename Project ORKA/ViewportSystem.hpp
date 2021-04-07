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
	Int absoluteX = 0;
	Int absoluteY = 0;
	Int absoluteWidth = 0;
	Int absoluteHeight = 0;

	void update(Int & adaptiveWidth, Int & adaptiveHeight);
	float aspectRatio();
	void render();
};

struct ViewportSystem {
	void create();
	void destroy();
	Viewport& current();
	void select(String name);
	void render(Framebuffer & framebuffer);
	void add(String name, Float x, Float y, Float w, Float h);

private:
	Vector<Viewport> viewports;
	Map<String, Index> viewportNames;
	Index currentViewport = 0;
};