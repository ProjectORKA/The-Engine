#pragma once

#include "OctreeSystem.hpp"
#include "QuadtreeSystem.hpp"
#include "TerrainSystem.hpp"

struct PlanetSystem
{
	OctreeSystem   octreeSystem;
	TerrainSystem  terrainSystem;
	QuadtreeSystem quadtreeSystem;

	void update();
	void destroy();
	void count() const;
	void create();
};
