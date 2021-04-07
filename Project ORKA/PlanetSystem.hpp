
#pragma once

#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"

struct PlanetSystem {

	OctreeNode octreeRoot;
	SharedPointer<QuadtreeNode> quadtreeRoot;

	void count();
	void create();
	void destroy();
	void update();
};