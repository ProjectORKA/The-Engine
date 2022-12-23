#pragma once

#include "Heightmap.hpp"
#include "Settings.hpp"

struct QuadtreeSystem;
struct QuadtreeNode;
struct QuadtreeID;
struct TerrainSystem;

struct Terrain {
	AdvancedHeightMap heightmap;
	Terrain(TerrainSystem& terrainSystem, QuadtreeID id, Terrain* parentTerrain, Bool cx, Bool cy);
};

struct HeightTexture {
	Float data[TERRAIN_TEXTURE_SIZE][TERRAIN_TEXTURE_SIZE] = {};
};

struct NormalTexture {
	Vec3 data[TERRAIN_TEXTURE_SIZE][TERRAIN_TEXTURE_SIZE] = {};
};

struct TerrainSystem {
	HeightTexture heightTextures[MAX_CHUNK_LEVEL];
	TerrainSystem();
};
