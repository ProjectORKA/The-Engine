#pragma once

#include "Debug.hpp"
#include "Entity.hpp"
#include "Terrain.hpp"

#define CHUNK_LEVEL_MAX 62					//the highest detail level the world system can go (max 62 62 = 63 + 1 extra precision for half way point)
#define CHUNK_DISTANCE_MULTIPLIER 1.001f

struct GameSimulation;
struct Renderer;

struct Chunk {
	ULLVec3 location = ULLVec3(0);
	UShort level = 0;
	Bool subdivided = false;
	UInt nextTicksInUse = 0;
	Mutex mutex;

	Terrain terrain;

	Chunk* tfr = nullptr;
	Chunk* tfl = nullptr;
	Chunk* tbr = nullptr;
	Chunk* tbl = nullptr;
	Chunk* bfr = nullptr;
	Chunk* bfl = nullptr;
	Chunk* bbr = nullptr;
	Chunk* bbl = nullptr;

	Chunk();
	Chunk(GameSimulation& gameSimulation, Chunk & parent, Bool x, Bool y, Bool z);
	~Chunk();

	void unsubdivide();
	void subdivide(GameSimulation& gameSimulation);
	void processSubdivision(GameSimulation& gamesimulation);
	void generateTerrain();
	void setIsInUse();
};