#pragma once

#include "PlanetSystem.hpp"
#include "QuadtreeSystemRenderer.hpp"
#include "OctreeSystemRenderer.hpp"
#include "Time.hpp"

struct PlanetRenderSystem {

	Bool chunkBorders = true;
	Bool worldDistortion = false;

	OctreeRenderSystem octreeRenderSystem;
	QuadtreeRenderSystem quadtreeRenderSystem;

	void destroy();
	void create(Renderer & renderer);
	void render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera& planetCamera);
};