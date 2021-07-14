
#pragma once

#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"

struct PlanetSystem {

	OctreeSystem octreeSystem;
	QuadtreeSystem quadtreeSystem;

	void count();
	void create();
	void destroy();
	void update();
};