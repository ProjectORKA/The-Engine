#pragma once

#include "Basics.hpp"
#include "OctreeSystemRenderer.hpp"
#include "QuadtreeSystemRenderer.hpp"

#include "Random.hpp"

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
	void renderLevel(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level);
};

struct ClipMap {
	GPUMesh mesh;

	Array2D<Float> heightmap = Array2D<Float>(TERRAIN_TEXTURE_SIZE);

	void render(Renderer& renderer);
	void update(PlanetSystem& planetSystem);
};

struct PlanetRenderSystem2 {

	Vector<ClipMap> clipMaps;

	void create(PlanetSystem& planetSystem);

	void update(PlanetSystem& planetSystem);

	void render(Renderer& renderer, PlanetSystemPlayer& player, UShort level);
};