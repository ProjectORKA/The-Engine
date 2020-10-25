#pragma once

#include "Debug.hpp"
#include "Entity.hpp"
#include "Terrain.hpp"

#define CHUNK_LEVEL_MAX 62					//the highest detail level the world system can go (max 62 62 = 63 + 1 extra precision for half way point)
#define CHUNK_DISTANCE_MULTIPLIER 1.001f
//#define SEALEVEL 0001010101010101010101010101010101010101010101010101010101010101


struct GameSimulation;

struct ChunkData {
	Terrain terrain;
	Vector<Index> entityIDs;
};

//struct WorldChunk {
//	//data
//	ChunkData worldData;
//
//	//user control
//	UInt nextTicksInUse = 0;
//	Mutex mutex;
//
//	//octree functionality
//	ULLVec3 location = ULLVec3(0);
//	UShort level = 0;				//0-63 0 = biggest level 63 = smallest level
//	Bool subdivided = false;
//
//	WorldChunk* tfr = nullptr;
//	WorldChunk* tfl = nullptr;
//	WorldChunk* tbr = nullptr;
//	WorldChunk* tbl = nullptr;
//	WorldChunk* bfr = nullptr;
//	WorldChunk* bfl = nullptr;
//	WorldChunk* bbr = nullptr;
//	WorldChunk* bbl = nullptr;
//
//	WorldChunk();
//	WorldChunk(WorldChunk & parent, Bool x, Bool y, Bool z);
//	~WorldChunk();
//
//	void unsubdivide();
//	void subdivide(GameSimulation& gameSimulation);
//	void generateEntities();
//	void setIsInUse();
//	
//	bool processSubdivision(GameSimulation& gamesimulation);
//	bool isInRenderDistance(ULLVec3 cameraLocation, UInt renderDistance);
//};