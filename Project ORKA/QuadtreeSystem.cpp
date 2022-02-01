
#include "QuadtreeSystem.hpp"

void QuadtreeSystem::create(TerrainSystem& terrainSystem) {
	root = new QuadtreeNode();
	root->create(terrainSystem);
}

void QuadtreeSystem::destroy() {
	delete root;
	root = nullptr;
}

void QuadtreeSystem::count() {
	root->count();
}
void QuadtreeSystem::update(TerrainSystem& terrainSystem) {
	root->update(terrainSystem);
}

ULLVec3 QuadtreeSystem::rayTrace(Ray ray) {
	return ULLVec3(0); // root.rayTrace(ray);
}

QuadtreeNode& QuadtreeSystem::get(TerrainSystem& terrainSystem,QuadtreeID id)
{
	return root->get(terrainSystem,id);
}
