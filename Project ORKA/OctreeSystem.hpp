#pragma once

#include "OctreeNode.hpp"
#include "QuadtreeSystem.hpp"

struct OctreeSystem {
	OctreeNode * root = nullptr;

	void count();
	void update();
	void destroy();
	void create(QuadtreeSystem& quadtreeSystem);
};