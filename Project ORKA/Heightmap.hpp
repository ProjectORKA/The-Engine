
#pragma once

#include "Basics.hpp"
#include "Settings.hpp"

struct AdvancedHeightMap {
	LDouble upperLimit = 0;
	LDouble lowerLimit = 0;
	LDouble height[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE] = {};
	Bool loaded = false;
};