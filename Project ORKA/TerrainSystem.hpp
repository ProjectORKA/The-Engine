
#pragma once

#include "Basics.hpp"
#include "Math.hpp"

struct Terrain {
	//data
	Float height = 0.0f;

	ULL locationX = 0;
	ULL locationY = 0;

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