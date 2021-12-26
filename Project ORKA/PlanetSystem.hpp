
#pragma once

#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"

struct PlanetSystem {
	OctreeSystem octreeSystem;
	QuadtreeSystem quadtreeSystem;

	void count();
	void update();
	PlanetSystem();
	~PlanetSystem();
};