#pragma once

#include "Math.hpp"
#include "Queue.hpp"
#include "ULLUtil.hpp"
#include "Heightmap.hpp"
#include "CPUTexture.hpp"
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

struct TerrainTile {
	//Bool isOcean = false;
	Short level = 0;
	Double height = 0;
	Float humidity = 0.5;
	Float hardness = 0.5;
	Float temperature = 25;
};

Vector<TerrainTile> generateTerrainTiles(TerrainTile input);
Vector<TerrainTile> generateSmoothTerrainTiles(TerrainTile input, TerrainTile front, TerrainTile back, TerrainTile left, TerrainTile right);;
