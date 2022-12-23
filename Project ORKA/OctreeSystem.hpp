#pragma once

#include "OctreeNode.hpp"

struct QuadtreeSystem;

struct OctreeSystem {
	OctreeNode * root = nullptr;

	void count();
	//void update();
	void destroy();
	void create(QuadtreeSystem& quadtreeSystem);
};