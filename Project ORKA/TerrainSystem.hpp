#pragma once

#include "Math.hpp"
#include "ULLUtil.hpp"
#include "Heightmap.hpp"
#include "QuadtreeID.hpp"

#define SEA_LEVEL 1677721600000
#define TERRAIN_GENERATION_SEED 645123587412588622

struct Terrain {
	HeightmapForNormals heightmapForNormals;
	
	ULL upperLimit = 0;
	ULL lowerLimit = 0;

	Mutex mutex;
	void create(QuadtreeID id);
};

struct TerrainSystem {
	Map<QuadtreeID, Terrain> terrain;
};

extern TerrainSystem terrainSystem;

Double terrainGenerationFunction(LDouble x, LDouble y);