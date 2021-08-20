#pragma once

#include "Basics.hpp"

struct OctreeNodeData {
	Bool isTerrain = false;
	Bool hasContent = false;

	Vector<Vec3> TerrainPoints;
};