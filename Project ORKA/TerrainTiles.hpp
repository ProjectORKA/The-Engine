
#pragma once

#include "Basics.hpp"

struct TerrainTile {
	Short level = 0;
	Double height = 0;
	Float humidity = 0.5;
	Float hardness = 0.5;
	Float temperature = 25;
};

Vector<TerrainTile> generateTerrainTiles(TerrainTile input);
Vector<TerrainTile> generateSmoothTerrainTiles(TerrainTile input, TerrainTile north, TerrainTile south, TerrainTile east, TerrainTile west);