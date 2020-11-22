#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "Math.hpp"
#include "TerrainSystem.hpp"

#define MAX_CHUNK_LEVEL 52			//if level 0 is the size of earth then we stop at level 52 to get sub millimeter precision
#define ONE_METER_CHUNK_LEVEL 39	//at this level 1 uit = 1 meter

struct WorldChunk {

	UShort level = 0;
	ULL xLocation = 0;
	ULL yLocation = 0;
	ULL zLocation = 0;

	UShort inUse = 0;

	Terrain terrain;

	WorldChunk* parentChunk = nullptr;
	bool subdivided = false;
	WorldChunk* c000 = nullptr;
	WorldChunk* c001 = nullptr;
	WorldChunk* c010 = nullptr;
	WorldChunk* c011 = nullptr;
	WorldChunk* c100 = nullptr;
	WorldChunk* c101 = nullptr;
	WorldChunk* c110 = nullptr;
	WorldChunk* c111 = nullptr;


	WorldChunk();
	WorldChunk(WorldChunk& parent, Bool x, Bool y, Bool z);
	~WorldChunk();
	
	void update();
	void isInUse();
	void subdivide();
	void unsubdivide();
	void count(Int& count);
};

struct PlanetSystem {

	WorldChunk chunk;
	TerrainSystem terrainSystem;

	void update();
	void count();
};