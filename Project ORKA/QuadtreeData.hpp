
#pragma once

#include "TerrainSystem.hpp"
#include "TreeSystem.hpp"

struct QuadtreeData {
	Terrain * terrain = nullptr;
	Vector<Tree> trees;
};