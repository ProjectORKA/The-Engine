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

	OctreeRenderSystem octreeRenderSystem;
	QuadtreeRenderSystem quadtreeRenderSystem;

	void destroy();
	void render(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player);
};