#pragma once

#include "Basics.hpp"

struct OctreeID {
	ULLVec3 location = ULLVec3(0);
	Short level = 0;

	Double size() {
		return pow(2,64- level);
	}
};