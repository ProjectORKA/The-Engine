#pragma once

#include "QuadtreeNode.hpp"

struct QuadTreeSystem {
	QuadtreeNode* root = nullptr;

	void count() const;
	ULLVec3 rayTrace(Ray ray);
	void update(TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, const QuadtreeID& id) const;
};
