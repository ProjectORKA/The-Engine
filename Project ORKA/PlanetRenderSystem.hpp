#pragma once

#include "PlanetSystem.hpp"
#include "QuadtreeSystemRenderer.hpp"
#include "OctreeSystemRenderer.hpp"
#include "Time.hpp"

struct PlanetRenderSystem {
	Bool chunkBorders = false;
	Bool worldDistortion = true;
	Float drawDistance = 2.0f;

	OctreeNodeRenderData octreeNodeRenderData;
	QuadtreeNodeRenderData quadtreeNodeRenderData;

	void destroy();
	void render(PlanetSystem& planetSystem, Renderer& renderer);
	void dynamicallyUpdateDrawDistance(Float targetFrameRate, Time& rendererTime);
};