
#pragma once

#include "Basics.hpp"
#include "Math.hpp"

struct Terrain {
	Bool hasTerrain = false;

	void generate(ULLVec3 location, UShort level);
};