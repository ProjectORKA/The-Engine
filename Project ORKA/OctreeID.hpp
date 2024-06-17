#pragma once

#include "Basics.hpp"

struct OctreeId
{
	Short   level    = 0;
	UllVec3 position = UllVec3(0);

	Double size() const;
};
