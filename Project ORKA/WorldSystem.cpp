
#include "Program.hpp"

//CHUNK
//subdivision
void unsubdivideChunk(Chunk & chunk) {
	if (chunk.subdivided) {
		delete chunk.tfr;
		delete chunk.tfl;
		delete chunk.tbr;
		delete chunk.tbl;
		delete chunk.bfr;
		delete chunk.bfl;
		delete chunk.bbr;
		delete chunk.bbl;

		chunk.tfr = nullptr;
		chunk.tfl = nullptr;
		chunk.tbr = nullptr;
		chunk.tbl = nullptr;
		chunk.bfr = nullptr;
		chunk.bfl = nullptr;
		chunk.bbr = nullptr;
		chunk.bbl = nullptr;

		chunk.subdivided = false;
	}
};
void subdivideChunk(Chunk & chunk) {
	if (chunk.subdivided == false && chunk.level < CHUNK_LEVEL_MAX) {
		std::cout << chunk.level << std::endl;
		unsigned short newLevel = chunk.level + 1;
		chunk.tfr = new Chunk(chunk, 1, 1, 1);
		chunk.tfl = new Chunk(chunk, 0, 1, 1);
		chunk.tbr = new Chunk(chunk, 1, 0, 1);
		chunk.tbl = new Chunk(chunk, 0, 0, 1);
		chunk.bfr = new Chunk(chunk, 1, 1, 0);
		chunk.bfl = new Chunk(chunk, 0, 1, 0);
		chunk.bbr = new Chunk(chunk, 1, 0, 0);
		chunk.bbl = new Chunk(chunk, 0, 0, 0);

		chunk.subdivided = true;
	}
};

Chunk::Chunk()
{
}
Chunk::Chunk(Chunk & chunk, bool x, bool y, bool z)
{
	this->level = chunk.level + 1;
	this->location.x = (chunk.location.x << 1) + x;
	this->location.y = (chunk.location.y << 1) + y;
	this->location.z = (chunk.location.z << 1) + z;
	this->parent = &chunk;
}
Chunk::~Chunk()
{
	unsubdivideChunk(*this);
}

//WORLD SYSTEM
Sky::Sky() {
	skyColor = glm::vec3(0.0f);
}

WorldSystem::WorldSystem()
{
	//spawn grid of all object types
	if (false) {
		int gridSize = 50;
		for (int x = 0; x < gridSize; x++) {
			for (int y = 0; y < gridSize; y++) {
				static int flopper = 0;
				spawnEntity(ecs, ecs.entityTypes.names[flopper]);
				if (ecs.entityTypes.structures[flopper][TRANSFORMATION]) {
					ecs.transformationSystem.transformations.back().location.x = 3.0f * (x - gridSize / 2.0f);
					ecs.transformationSystem.transformations.back().location.y = 3.0f * (y - gridSize / 2.0f);
				}
				flopper++;
				if (flopper >= ecs.entityTypes.numberOfEntityTypes) {
					flopper = 0;
				}
			}
		}
	}
}