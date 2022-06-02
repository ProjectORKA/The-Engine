#pragma once

#include "Basics.hpp"

struct Renderer;

struct RectangleRenderer{
	void render(Renderer& renderer, Float x, Float y, Float w, Float h);
};