#pragma once

#include "Math.hpp"
#include "Queue.hpp"
#include "ULLUtil.hpp"
#include "Heightmap.hpp"
#include "CPUTexture.hpp"

#define SEALEVEL 1677721600000
#define TERRAIN_GENERATION_SEED 645123587412588622

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
	HeightTexture heightTexture;
	//NormalTexture normalTexture;
	TerrainSystem();
};