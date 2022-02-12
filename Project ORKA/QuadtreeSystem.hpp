#pragma once

#include "QuadtreeNode.hpp"

struct QuadtreeSystem {
	QuadtreeNode * root = nullptr;

	void count();
	ULLVec3 rayTrace(Ray ray);
	void update(TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, QuadtreeID id);
};