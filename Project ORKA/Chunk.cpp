
#include "Chunk.hpp"
#include "Renderer.hpp"
#include "PerlinNoise.hpp"

void Chunk::processSubdivision(GameSimulation& gameSimulation) {
	if (subdivided) {
		if (nextTicksInUse > 0) {
			nextTicksInUse--;
			tfr->processSubdivision(gameSimulation);
			tfl->processSubdivision(gameSimulation);
			tbr->processSubdivision(gameSimulation);
			tbl->processSubdivision(gameSimulation);
			bfr->processSubdivision(gameSimulation);
			bfl->processSubdivision(gameSimulation);
			bbr->processSubdivision(gameSimulation);
			bbl->processSubdivision(gameSimulation);
		}
		else {
			unsubdivide();
		}
	}
	else {
		if (nextTicksInUse > 0) {
			subdivide(gameSimulation);
			nextTicksInUse--;
		}
		else {
			unsubdivide();
		}
	}
}
void Chunk::unsubdivide() {
	if (subdivided) {
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

void Chunk::subdivide(GameSimulation& gameSimulation) {
	mutex.lock();
	logEvent(String("Subdividing!"));
	if (subdivided == false && level < CHUNK_LEVEL_MAX) {
		tfr = new Chunk(gameSimulation, *this, 1, 1, 1);
		tfl = new Chunk(gameSimulation, *this, 0, 1, 1);
		tbr = new Chunk(gameSimulation, *this, 1, 0, 1);
		tbl = new Chunk(gameSimulation, *this, 0, 0, 1);
		bfr = new Chunk(gameSimulation, *this, 1, 1, 0);
		bfl = new Chunk(gameSimulation, *this, 0, 1, 0);
		bbr = new Chunk(gameSimulation, *this, 1, 0, 0);
		bbl = new Chunk(gameSimulation, *this, 0, 0, 0);
		subdivided = true;
	}
	mutex.unlock();
};
void Chunk::generateTerrain() {
	static PerlinNoise noise;

	noise.reseed(12345);

	long double noiseSize = 1;

	long double x = (location.x << 64 - level) / (long double(ULLONG_MAX) / noiseSize);
	long double y = (location.y << 64 - level) / (long double(ULLONG_MAX) / noiseSize);

	unsigned long long target = unsigned long long(long double(noise.octaveNoise0_1(x, y,8)) * long double(LLONG_MAX)/8);

	terrain.hasTerrain = location.z == 0;// (location.z << 64 - level < target);
}
void Chunk::setIsInUse() {
	nextTicksInUse = 1000;
}
Chunk::Chunk(){};
Chunk::Chunk(GameSimulation& gameSimualtion, Chunk & parent, Bool x, Bool y, Bool z) {
	level = parent.level + 1;
	location.x = (parent.location.x << 1) + x;
	location.y = (parent.location.y << 1) + y;
	location.z = (parent.location.z << 1) + z;

	//transferEntities(chunk, parent, gameSimulation,x,y,z);

	//generateEntities(chunk, gameSimulation);

	generateTerrain();
}
Chunk::~Chunk()
{
	unsubdivide();
}