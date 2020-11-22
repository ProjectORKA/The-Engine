#pragma once

#include "GPUMesh.hpp"
#include "PlanetSystem.hpp"

struct TerrainRenderInfo {
	GPUMesh terrainMesh;

	UShort level = 0;

	bool subdivided = false;
	TerrainRenderInfo* tri00 = nullptr;
	TerrainRenderInfo* tri01 = nullptr;
	TerrainRenderInfo* tri10 = nullptr;
	TerrainRenderInfo* tri11 = nullptr;

	void subdivide();
	void create(WorldChunk& chunk);
	void update(WorldChunk& chunk);
	void render();
};


struct TerrainRenderSystem {
	TerrainRenderInfo terrainRenderInfo;

	void create(WorldChunk& chunk);
	void update(WorldChunk& chunk);
};