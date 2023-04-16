
#include "PlanetSystem.hpp"

void PlanetSystem::create(ResourceManager& resourceManager)
{
	quadtreeSystem.create(terrainSystem);
	octreeSystem.create(quadtreeSystem);
	terrainSystem.create(resourceManager);
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
