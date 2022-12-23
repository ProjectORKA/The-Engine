
#pragma once

#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"
#include "TerrainSystem.hpp"

struct PlanetSystem {
	//OctreeSystem octreeSystem;
	QuadtreeSystem quadtreeSystem;
	TerrainSystem terrainSystem;

	void count();
	//void update();
	PlanetSystem();
	~PlanetSystem();
};