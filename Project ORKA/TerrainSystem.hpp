#pragma once

#include "Heightmap.hpp"

// #define SEALEVEL 1677721600000
#define TERRAIN_GENERATION_SEED 645123587412588622

struct QuadtreeSystem;
struct QuadtreeNode;
struct QuadtreeId;
struct TerrainSystem;

struct Terrain
{
	AdvancedHeightMap&             getHeightmap();
	[[nodiscard]] LDouble          getLowerLimit() const;
	[[nodiscard]] LDouble          getUpperLimit() const;
	[[nodiscard]] const LDouble (& getHeights() const)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE];
	Terrain(const TerrainSystem& terrainSystem, const QuadtreeId& id, const Terrain* parentTerrain, Bool cx, Bool cy);
	~Terrain();

private:
	AdvancedHeightMap heightmap;
};

struct HeightTexture
{
	[[nodiscard]] Float getHeight(Int x, Int y) const;
	void                setHeight(UInt x, UInt y, Float value);

private:
	Float data[TERRAIN_TEXTURE_SIZE][TERRAIN_TEXTURE_SIZE] = {};
};

struct TerrainSystem
{
	HeightTexture heightTextures[MAX_CHUNK_LEVEL];
	void          create();
};
