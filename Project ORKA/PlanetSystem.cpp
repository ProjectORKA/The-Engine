
#include "PlanetSystem.hpp"

PlanetSystem::PlanetSystem()
{
	quadtreeSystem.create();
	octreeSystem.create(quadtreeSystem);
}

PlanetSystem::~PlanetSystem() {
	octreeSystem.destroy();
	quadtreeSystem.destroy();
}

//planetSystem
void PlanetSystem::count()
{
	octreeSystem.count();
	quadtreeSystem.count();
}
void PlanetSystem::update()
{
	quadtreeSystem.update();
	octreeSystem.update();
}
