
#include "PlanetSystem.hpp"

//planetSystem
void PlanetSystem::count()
{
	octreeRoot.count();
	quadtreeRoot->count();
}
void PlanetSystem::create()
{
	quadtreeRoot = std::make_shared<QuadtreeNode>();
	quadtreeRoot->createRootNode();
	octreeRoot.createRootNode(quadtreeRoot);
}
void PlanetSystem::destroy()
{
	octreeRoot.destroy();
	quadtreeRoot.reset();
}

void PlanetSystem::update()
{
	quadtreeRoot->update();
	octreeRoot.update();
}
