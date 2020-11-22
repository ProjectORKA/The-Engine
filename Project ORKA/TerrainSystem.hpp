
#pragma once

#include "Basics.hpp"
#include "Math.hpp"

#define TERRAIN_DETAIL 16

struct Terrain {
	//data
	Float heightmap[TERRAIN_DETAIL+1][TERRAIN_DETAIL+1]{};

	UShort level = 0;

	Terrain* parent = nullptr;
	//children
	Terrain* c00 = nullptr;
	Terrain* c01 = nullptr;
	Terrain* c10 = nullptr;
	Terrain* c11 = nullptr;

	void create();
};

struct TerrainSystem {
	Terrain terrain;

	void create(); //[TODO] make seed dependent
};