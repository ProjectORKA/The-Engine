#pragma once

#include "Entity.hpp"
#include "Terrain.hpp"

#define CHUNK_LEVEL_MAX 62					//the highest detail level the world system can go (max 62 62 = 63 + 1 extra precision for half way point)

struct GameSimulation;

struct Chunk {
	//Vector<Entity> entityIDs;

	ULLVec3 location = ULLVec3(0);
	UShort level = 0;
	Bool subdivided = false;
	UInt nextTicksInUse = 0;
	Mutex mutex;

	Terrain terrain;

	Chunk* parent = nullptr;

	std::shared_ptr<Chunk> tfr = nullptr;
	std::shared_ptr<Chunk> tfl = nullptr;
	std::shared_ptr<Chunk> tbr = nullptr;
	std::shared_ptr<Chunk> tbl = nullptr;
	std::shared_ptr<Chunk> bfr = nullptr;
	std::shared_ptr<Chunk> bfl = nullptr;
	std::shared_ptr<Chunk> bbr = nullptr;
	std::shared_ptr<Chunk> bbl = nullptr;
};

void generateTerrain(Chunk& chunk);

void subdivideChunk(Chunk& chunk, GameSimulation& gameSimulation);

void setChunkIsInUse(Chunk& chunk, GameSimulation& gameSimulation);

void processSubdivision(Chunk& chunk, GameSimulation& gameSimulation);

void processWorldSystem(GameSimulation& gameSimulation);

void createChildChunk(Chunk& chunk, Chunk& parent, GameSimulation& gameSimulation, bool x, bool y, bool z);