#pragma once

#include "Math.hpp"
#include "ULLUtil.hpp"
#include "Heightmap.hpp"
#include "Queue.hpp"

#define SEALEVEL 1677721600000
#define TERRAIN_GENERATION_SEED 645123587412588622

struct QuadtreeSystem;
struct QuadtreeNode;
struct QuadtreeID;

struct Terrain {
	HeightmapForNormals heightmapForNormals;
	
	ULL upperLimit = 0;
	ULL lowerLimit = 0;

	//Mutex mutex;
	Terrain(QuadtreeID id);
};

Double terrainGenerationFunction(LDouble x, LDouble y);