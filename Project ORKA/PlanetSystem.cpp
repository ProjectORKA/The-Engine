
#include "PlanetSystem.hpp"

//planetSystem
void PlanetSystem::count()
{
	octreeSystem.count();
	quadtreeSystem.count();
}
void PlanetSystem::create()
{
	quadtreeSystem.create();
	octreeSystem.create(quadtreeSystem);
}
void PlanetSystem::destroy()
{
	octreeSystem.destroy();
	quadtreeSystem.destroy();
}
void PlanetSystem::update()
{
	quadtreeSystem.update();
	//quadtreeSystem.count();
	octreeSystem.update();
}
