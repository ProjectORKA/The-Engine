#pragma once

#include "Basics.hpp"

struct OctreeId
{
	Short   level    = 0;
	ULLVec3 location = ULLVec3(0);

	Double size() const;
};
