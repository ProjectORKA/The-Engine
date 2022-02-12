
#include "QuadtreeSystem.hpp"

void QuadtreeSystem::count() {
	root->count();
}
ULLVec3 QuadtreeSystem::rayTrace(Ray ray) {
	return ULLVec3(0);
}
void QuadtreeSystem::create(TerrainSystem& terrainSystem) {
	root = new QuadtreeNode();
	root->create(terrainSystem);
}
void QuadtreeSystem::update(TerrainSystem& terrainSystem) {
	root->update(*this, terrainSystem);
}
QuadtreeNode& QuadtreeSystem::get(TerrainSystem& terrainSystem,QuadtreeID id)
{
	return root->get(terrainSystem,id);
}