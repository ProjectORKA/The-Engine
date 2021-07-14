#pragma once

#include "PlanetSystem.hpp"
#include "QuadtreeSystemRenderer.hpp"
#include "OctreeSystemRenderer.hpp"
#include "Time.hpp"

struct PlanetRenderSystem {

	OctreeRenderSystem octreeRenderSystem;
	QuadtreeRenderSystem quadtreeRenderSystem;

	void destroy();
	void create(Renderer & renderer);
	//void dynamicallyUpdateDrawDistance(Float targetFrameRate, Time& rendererTime);
	void render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera& planetCamera);
};