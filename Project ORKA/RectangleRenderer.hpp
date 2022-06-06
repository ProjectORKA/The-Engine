#pragma once

#include "Basics.hpp"

struct Renderer;

struct RectangleRenderer{
	Index rectangleMeshId = -1;

	void create(Renderer& renderer);
	void render(Renderer& renderer, Vec2 pos, Vec2 size);
};