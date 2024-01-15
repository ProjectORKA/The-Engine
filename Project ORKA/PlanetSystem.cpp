#include "PlanetSystem.hpp"

void PlanetSystem::update()
{
	quadtreeSystem.update(terrainSystem);
	octreeSystem.update();
}

void PlanetSystem::destroy()
{
	octreeSystem.destroy();
	quadtreeSystem.destroy();
}

void PlanetSystem::count() const
{
	octreeSystem.count();
	quadtreeSystem.count();
}

void PlanetSystem::create()
{
	terrainSystem.create();

	quadtreeSystem.create(terrainSystem);
	octreeSystem.create(quadtreeSystem);
}
