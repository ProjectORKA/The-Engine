#pragma once

#include "Basics.hpp"
#include "OctreeSystemRenderer.hpp"
#include "QuadtreeSystemRenderer.hpp"

struct Renderer;
struct PlanetSystem;
struct PlanetSystemPlayer;

struct PlanetRenderSystem {
	Bool chunkBorders = true;
	Bool worldDistortion = true;
	QuadtreeRenderSystem quadtreeRenderSystem;

	void destroy();
	void update(PlanetSystem& planetSystem, PlanetSystemPlayer& player);
	void render(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player);
	void renderLevel(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level);
};