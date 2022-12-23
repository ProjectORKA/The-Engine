
#pragma once

#include "TerrainTiles.hpp"
#include "TerrainSystem.hpp"

struct QuadtreeData {
	TerrainTile terrainTile;
	Terrain * terrain = nullptr;
};