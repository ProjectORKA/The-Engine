#include "QuadtreeSystem.hpp"

void QuadtreeSystem::count() const
{
	root->count();
}

void QuadtreeSystem::destroy() const
{
	root->destroy();
	free(root);
}

void QuadtreeSystem::update(TerrainSystem& terrainSystem)
{
	root->update(*this, terrainSystem);
}

void QuadtreeSystem::create(const TerrainSystem& terrainSystem)
{
	root = new QuadtreeNode();
	root->createRootNode(terrainSystem);
}

QuadtreeNode& QuadtreeSystem::get(TerrainSystem& terrainSystem, const QuadtreeId& id) const
{
	return root->get(terrainSystem, id);
}
