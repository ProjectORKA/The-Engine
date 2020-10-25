#pragma once

#include "Basics.hpp"
#include "Math.hpp"

struct WorldChunk {

	UShort level = 0;
	ULL xLocation = 0;
	ULL yLocation = 0;

	Float terrainHeight = 0.0f;

	bool subdivided = false;
	WorldChunk* c00 = nullptr;
	WorldChunk* c01 = nullptr;
	WorldChunk* c10 = nullptr;
	WorldChunk* c11 = nullptr;

	WorldChunk();
	WorldChunk(WorldChunk& parent, bool x, bool y);
	void subdivide();
	void unsubdivide();
};

struct PlanetSystem {

	WorldChunk chunk;

	void process();
};