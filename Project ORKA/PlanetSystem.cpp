
#include "PlanetSystem.hpp"

void PlanetSystem::process() {
	chunk.subdivide();

	chunk.c11->subdivide();

}

WorldChunk::WorldChunk()
{
	terrainHeight = randomFloat();
}

WorldChunk::WorldChunk(WorldChunk& parent, bool x, bool y)
{
	level = parent.level + 1;
	xLocation = parent.xLocation << 1;
	yLocation = parent.yLocation << 1;
	if (x) xLocation++;
	if (y) yLocation++;
}

void WorldChunk::subdivide()
{
	if (!subdivided) {
		c00 = new WorldChunk(*this, 0, 0);
		c01 = new WorldChunk(*this, 0, 1);
		c10 = new WorldChunk(*this, 1, 0);
		c11 = new WorldChunk(*this, 1, 1);
		subdivided = true;
	}
}

void WorldChunk::unsubdivide()
{
	if (subdivided) {
		delete c00;
		delete c01;
		delete c10;
		delete c11;
		c00 = nullptr;
		c01 = nullptr;
		c10 = nullptr;
		c11 = nullptr;
	}
}