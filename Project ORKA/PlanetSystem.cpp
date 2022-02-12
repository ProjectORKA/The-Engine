
#include "PlanetSystem.hpp"

PlanetSystem::PlanetSystem()
{
	quadtreeSystem.create(terrainSystem);
	octreeSystem.create(quadtreeSystem);
}
PlanetSystem::~PlanetSystem() {
	octreeSystem.destroy();
}

//planetSystem
void PlanetSystem::count()
{
	octreeSystem.count();
	quadtreeSystem.count();
}
void PlanetSystem::update()
{
	quadtreeSystem.update(terrainSystem);
	octreeSystem.update();
}
