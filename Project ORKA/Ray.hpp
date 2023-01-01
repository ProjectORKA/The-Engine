
#pragma once

#include "Basics.hpp"

struct Ray {
	ULLVec3 origin;
	DVec3 direction;

	void advance(Double distance);;
};