#pragma once
#include "OctreeSystem.hpp"
#include "TerrainSystem.hpp"
#include "QuadtreeSystem.hpp"

struct PlanetSystem
{
	OctreeSystem   octreeSystem;
	TerrainSystem  terrainSystem;
	QuadtreeSystem quadtreeSystem;

	void update();
	void create();
	void destroy();
	void count() const;
};
