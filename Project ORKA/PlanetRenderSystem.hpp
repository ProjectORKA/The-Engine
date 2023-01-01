#pragma once

#include "Basics.hpp"
#include "OctreeSystemRenderer.hpp"
#include "QuadtreeSystemRenderer.hpp"

struct Renderer;
struct PlanetSystem;
struct PlanetSystemPlayer;

struct PlanetRenderSystem {
	Bool chunkBorders = true;
	Bool vertexColors = false;
	Bool worldDistortion = true;
	QuadtreeRenderSystem quadtreeRenderSystem;

	void destroy();
	void update(PlanetSystem& planetSystem, PlanetSystemPlayer& player);
	void renderLevel(Engine& engine, PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level);
	void renderAllLevels(Engine & engine, PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player);
};