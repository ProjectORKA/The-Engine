#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"

void OctreeSystem::destroy()
{
	delete root;
	root = nullptr;
}

void OctreeSystem::count() const
{
	root->count();
}

void OctreeSystem::update() const
{
	root->update();
}

void OctreeSystem::create(const QuadtreeSystem& quadTreeSystem)
{
	root = new OctreeNode(*quadTreeSystem.root);
}
