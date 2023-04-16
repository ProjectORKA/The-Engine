
#pragma once

#include "Basics.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct RectangleRenderer{
	Index rectangleMeshId = -1;

	void create(ResourceManager& resourceManager, Renderer& renderer);
	void render(ResourceManager& resourceManager, Renderer& renderer, Vec2 pos, Vec2 size);
};