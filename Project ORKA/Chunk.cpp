
#include "Chunk.hpp"
#include "Math.hpp"

//UInt subCounter = 0;

bool WorldChunk::processSubdivision(GameSimulation& gameSimulation) {
	if (subdivided) {
		if (nextTicksInUse > 0) {
			Int tmp = 
			tfr->processSubdivision(gameSimulation) +
			tfl->processSubdivision(gameSimulation) +
			tbr->processSubdivision(gameSimulation) +
			tbl->processSubdivision(gameSimulation) +
			bfr->processSubdivision(gameSimulation) +
			bfl->processSubdivision(gameSimulation) +
			bbr->processSubdivision(gameSimulation) +
			bbl->processSubdivision(gameSimulation);
			if (tmp > 0) return 1;
		}
		else {
			unsubdivide();
		}
	}
	else {
		if (nextTicksInUse > 0) {
			subdivide(gameSimulation);
			nextTicksInUse--;
			return 1;
		}
		else {
			unsubdivide();
		}
	}
	return 0;
}
void WorldChunk::unsubdivide() {
	if (subdivided) {
		//unsubdivide existing chunks
		tfr->unsubdivide();
		tfl->unsubdivide();
		tbr->unsubdivide();
		tbl->unsubdivide();
		bfr->unsubdivide();
		bfl->unsubdivide();
		bbr->unsubdivide();
		bbl->unsubdivide();

		//lock and delete pointers
		mutex.lock();
		subdivided = false;
		delete tfr;
		delete tfl;
		delete tbr;
		delete tbl;
		delete bfr;
		delete bfl;
		delete bbr;
		delete bbl;
		tfr = nullptr;
		tfl = nullptr;
		tbr = nullptr;
		tbl = nullptr;
		bfr = nullptr;
		bfl = nullptr;
		bbr = nullptr;
		bbl = nullptr;
		mutex.unlock();
	}
}

#include <thread>

void WorldChunk::subdivide(GameSimulation& gameSimulation) {
	mutex.lock();
	logDebug(String("Subdividing!"));
	if (subdivided == false && level < CHUNK_LEVEL_MAX) {
		tfr = new WorldChunk(*this, 1, 1, 1);
		tfl = new WorldChunk(*this, 0, 1, 1);
		tbr = new WorldChunk(*this, 1, 0, 1);
		tbl = new WorldChunk(*this, 0, 0, 1);
		bfr = new WorldChunk(*this, 1, 1, 0);
		bfl = new WorldChunk(*this, 0, 1, 0);
		bbr = new WorldChunk(*this, 1, 0, 0);
		bbl = new WorldChunk(*this, 0, 0, 0);
		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		subdivided = true;
	}
	mutex.unlock();
};

void WorldChunk::generateEntities()
{
	if(level > 29 && level < 38) worldData.entityIDs.push_back(0);
}
void WorldChunk::setIsInUse() {
	nextTicksInUse = 10000;
}

bool WorldChunk::isInRenderDistance(ULLVec3 cameraLocation, UInt renderDistance)
{

	//get camera location
	ULLVec3 distance = cameraLocation;

	//convert it to real coordinates
	distance.x = distance.x >> (62 - level);
	distance.y = distance.y >> (62 - level);
	distance.z = distance.z >> (62 - level);

	//get actual distance to camera
	distance -= location;
	
	//account for overflow
	if (distance.x > ULLONG_MAX >> level) distance.x = 0 - distance.x;
	if (distance.y > ULLONG_MAX >> level) distance.y = 0 - distance.y;
	if (distance.z > ULLONG_MAX >> level) distance.z = 0 - distance.z;

	//check if distance is acceptable for rendering
	return ((distance.x < renderDistance) && (distance.y < renderDistance) && (distance.z < renderDistance));
}

WorldChunk::WorldChunk(){};
WorldChunk::WorldChunk(WorldChunk & parent, Bool x, Bool y, Bool z) {
	level = parent.level + 1;
	location.x = (parent.location.x << 1) + x;
	location.y = (parent.location.y << 1) + y;
	location.z = (parent.location.z << 1) + z;

	generateEntities();

	worldData.terrain.generate(location, level);
}
WorldChunk::~WorldChunk()
{
	unsubdivide();
}