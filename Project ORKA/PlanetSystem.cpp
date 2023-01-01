
#include "PlanetSystem.hpp"

void PlanetSystem::create(Engine& engine)
{
	quadtreeSystem.create(terrainSystem);
	octreeSystem.create(quadtreeSystem);
	terrainSystem.create(engine);
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
