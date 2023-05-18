#pragma once

#include "OcTreeSystem.hpp"
#include "QuadTreeSystem.hpp"
#include "TerrainSystem.hpp"

struct PlanetSystem {
	OcTreeSystem octreeSystem;
	QuadTreeSystem quadtreeSystem;
	TerrainSystem terrainSystem;

	void count();
	void update();
	void create(ResourceManager& resourceManager);
	~PlanetSystem();
};
