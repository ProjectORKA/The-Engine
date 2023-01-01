
#pragma once

#include "Basics.hpp"
#include "Engine.hpp"

struct Renderer;

struct RectangleRenderer{
	Index rectangleMeshId = -1;

	void create(Engine & engine, Renderer& renderer);
	void render(Engine & engine, Renderer& renderer, Vec2 pos, Vec2 size);
};