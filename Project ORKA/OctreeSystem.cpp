
#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"

void OctreeSystem::count() {
	root->count();
}
void OctreeSystem::update() {
	root->update();
}
void OctreeSystem::destroy()
{
	delete  root;
	root = nullptr;
}
void OctreeSystem::create(QuadtreeSystem& quadtreeSystem) {
	root = new OctreeNode(*quadtreeSystem.root);
}