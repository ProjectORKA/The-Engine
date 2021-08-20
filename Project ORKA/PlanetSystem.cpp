
#include "PlanetSystem.hpp"

//planetSystem
void PlanetSystem::count()
{
	octreeSystem.count();
	quadtreeSystem.count();
}
void PlanetSystem::create()
{
	octreeSystem.create(quadtreeSystem);
}
void PlanetSystem::destroy()
{
	octreeSystem.destroy();
}
void PlanetSystem::update()
{
	quadtreeSystem.update();
	octreeSystem.update();
}
