
#include "Chunk.hpp"

void generateTerrain(Chunk& chunk) {
	static PerlinNoise noise;

	noise.reseed(12345);

	long double noiseSize = 32;

	long double x = ((unsigned long long)(chunk.location.x << (64 - chunk.level))) / ((long double)LLONG_MAX / noiseSize);
	long double y = ((unsigned long long)(chunk.location.y << (64 - chunk.level))) / ((long double)LLONG_MAX / noiseSize);
	long double z = (unsigned long long)(chunk.location.z << (64 - chunk.level));

	long double lowBorder = (long double)(((unsigned long long)(chunk.location.z)) << (64 - chunk.level));
	long double highBorder = (long double)(((unsigned long long)(chunk.location.z + 1)) << (64 - chunk.level));
	long double target = ((long double)noise.octaveNoise0_1(x, y, 16)) * ((long double)LLONG_MAX / ((long double)4.0 * noiseSize));

	if (chunk.location.z == 0) {
		chunk.terrain.hasTerrain = true;
		chunk.terrain.height = (float)(target / pow(2, 64 - chunk.level));
	}
}

void unsubdivideChunk(Chunk& chunk) {
	chunk.mutex.lock();
	chunk.subdivided = false;
	chunk.tfr = nullptr;
	chunk.tfl = nullptr;
	chunk.tbr = nullptr;
	chunk.tbl = nullptr;
	chunk.bfr = nullptr;
	chunk.bfl = nullptr;
	chunk.bbr = nullptr;
	chunk.bbl = nullptr;
	chunk.mutex.unlock();
};

void subdivideChunk(Chunk& chunk, GameSimulation& gameSimulation) {
	if (chunk.subdivided == false && chunk.level < CHUNK_LEVEL_MAX) {
		chunk.tfr = std::make_shared<Chunk>();
		chunk.tfl = std::make_shared<Chunk>();
		chunk.tbr = std::make_shared<Chunk>();
		chunk.tbl = std::make_shared<Chunk>();
		chunk.bfr = std::make_shared<Chunk>();
		chunk.bfl = std::make_shared<Chunk>();
		chunk.bbr = std::make_shared<Chunk>();
		chunk.bbl = std::make_shared<Chunk>();
		createChildChunk(*chunk.tfr, chunk, gameSimulation, 1, 1, 1);
		createChildChunk(*chunk.tfl, chunk, gameSimulation, 0, 1, 1);
		createChildChunk(*chunk.tbr, chunk, gameSimulation, 1, 0, 1);
		createChildChunk(*chunk.tbl, chunk, gameSimulation, 0, 0, 1);
		createChildChunk(*chunk.bfr, chunk, gameSimulation, 1, 1, 0);
		createChildChunk(*chunk.bfl, chunk, gameSimulation, 0, 1, 0);
		createChildChunk(*chunk.bbr, chunk, gameSimulation, 1, 0, 0);
		createChildChunk(*chunk.bbl, chunk, gameSimulation, 0, 0, 0);
		chunk.subdivided = true;
	}
};

void setChunkIsInUse(Chunk& chunk, GameSimulation& gameSimulation) {
	chunk.nextTicksInUse = 100;
}

void processSubdivision(Chunk& chunk, GameSimulation& gameSimulation) {
	if (chunk.subdivided) {
		if (chunk.nextTicksInUse > 0) {
			chunk.nextTicksInUse--;
			processSubdivision(*chunk.tfr, gameSimulation);
			processSubdivision(*chunk.tfl, gameSimulation);
			processSubdivision(*chunk.tbr, gameSimulation);
			processSubdivision(*chunk.tbl, gameSimulation);
			processSubdivision(*chunk.bfr, gameSimulation);
			processSubdivision(*chunk.bfl, gameSimulation);
			processSubdivision(*chunk.bbr, gameSimulation);
			processSubdivision(*chunk.bbl, gameSimulation);
		}
		else {
			unsubdivideChunk(chunk);
		}
	}
	else {
		if (chunk.nextTicksInUse > 0) {
			subdivideChunk(chunk, gameSimulation);
			chunk.nextTicksInUse--;
		}
		else {
			unsubdivideChunk(chunk);
		}
	}
}

void createChildChunk(Chunk& chunk, Chunk& parent, GameSimulation& gameSimulation, bool x, bool y, bool z) {
	chunk.parent = &parent;
	//chunk.locationInParentChunkX = x;
	//chunk.locationInParentChunkY = y;
	//chunk.locationInParentChunkZ = z;

	chunk.level = parent.level + 1;
	chunk.location.x = (parent.location.x << 1) + x;
	chunk.location.y = (parent.location.y << 1) + y;
	chunk.location.z = (parent.location.z << 1) + z;

	//transferEntities(chunk, parent, gameSimulation,x,y,z);

	//generateEntities(chunk, gameSimulation);
	generateTerrain(chunk);
}