
#include "PlanetSystem.hpp"

#define CHUNK_USAGE_TICK_COUNT 200

//worldChunk
void WorldChunk::isInUse()
{
	inUse = CHUNK_USAGE_TICK_COUNT;
}
void WorldChunk::update()
{
	if (inUse) {
		if (subdivided) {
			c000->update();
			c001->update();
			c010->update();
			c011->update();
			c100->update();
			c101->update();
			c110->update();
			c111->update();
		}
		else {
			subdivide();
		}
		inUse--;
	}
	else {
		unsubdivide();
	}
}
void WorldChunk::subdivide()
{
	if ((!subdivided) && (inUse) && (level < MAX_CHUNK_LEVEL)) {
		c000 = new WorldChunk(*this, 0, 0, 0);
		c001 = new WorldChunk(*this, 0, 0, 1);
		c010 = new WorldChunk(*this, 0, 1, 0);
		c011 = new WorldChunk(*this, 0, 1, 1);
		c100 = new WorldChunk(*this, 1, 0, 0);
		c101 = new WorldChunk(*this, 1, 0, 1);
		c110 = new WorldChunk(*this, 1, 1, 0);
		c111 = new WorldChunk(*this, 1, 1, 1);
		subdivided = true;
	}
}
void WorldChunk::unsubdivide()
{
	if (subdivided) {
		delete c000;
		delete c001;
		delete c010;
		delete c011;
		delete c100;
		delete c101;
		delete c110;
		delete c111;
		c000 = nullptr;
		c001 = nullptr;
		c010 = nullptr;
		c011 = nullptr;
		c100 = nullptr;
		c101 = nullptr;
		c110 = nullptr;
		c111 = nullptr;
		subdivided = false;
	}

}
void WorldChunk::count(Int& count)
{
	count++;
	if (subdivided) {
		c000->count(count);
		c001->count(count);
		c010->count(count);
		c011->count(count);
		c100->count(count);
		c101->count(count);
		c110->count(count);
		c111->count(count);
	}
}

WorldChunk::WorldChunk()
{
	terrain.create();
}
WorldChunk::WorldChunk(WorldChunk& parent, Bool x, Bool y, Bool z)
{
	parentChunk = &parent;
	level = parent.level + 1;
	xLocation = parent.xLocation << 1;
	yLocation = parent.yLocation << 1;
	zLocation = parent.zLocation << 1;
	xLocation += x;
	yLocation += y;
	zLocation += z;
	inUse = 0;

	terrain.create();
}
WorldChunk::~WorldChunk()
{
	unsubdivide();
}

//planetSystem
void PlanetSystem::count()
{
	Int chunkCount = 0;
	chunk.count(chunkCount);
	logDebug(chunkCount);
}
void PlanetSystem::update() {
	chunk.update();
	//count();
}