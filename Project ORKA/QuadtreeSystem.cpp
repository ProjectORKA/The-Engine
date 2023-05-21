#include "QuadTreeSystem.hpp"

void QuadTreeSystem::count() const { root->count(); }
ULLVec3 QuadTreeSystem::rayTrace(Ray ray) { return ULLVec3(0); }

void QuadTreeSystem::create(TerrainSystem& terrainSystem) {
	root = new QuadtreeNode();
	root->createRootNode(terrainSystem);
}

void QuadTreeSystem::update(TerrainSystem& terrainSystem) { root->update(*this, terrainSystem); }

QuadtreeNode& QuadTreeSystem::get(TerrainSystem& terrainSystem, const QuadtreeId& id) const {
	return root->get(terrainSystem, id);
}
