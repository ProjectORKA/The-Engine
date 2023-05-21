#pragma once

#include "Math.hpp"
#include "Queue.hpp"
#include "ULLUtil.hpp"
#include "Heightmap.hpp"
#include "CPUTexture.hpp"

#define SEALEVEL 1677721600000
#define TERRAIN_GENERATION_SEED 645123587412588622

struct QuadTreeSystem;
struct QuadtreeNode;
struct QuadtreeId;
struct TerrainSystem;

struct Terrain {
	AdvancedHeightMap heightmap;
	Terrain(const TerrainSystem& terrainSystem, const QuadtreeId& id, const Terrain* parentTerrain, Bool cx, Bool cy);
};

struct HeightTexture {
	Float data[TERRAIN_TEXTURE_SIZE][TERRAIN_TEXTURE_SIZE] = {};
};

struct NormalTexture {
	Vec3 data[TERRAIN_TEXTURE_SIZE][TERRAIN_TEXTURE_SIZE] = {};
};

struct TerrainSystem {
	HeightTexture heightTextures[MAX_CHUNK_LEVEL];
	void create(ResourceManager& resourceManager);
};
