#pragma once

#include "QuadtreeNode.hpp"

struct QuadtreeSystem {
	QuadtreeNode root;
	void count();
	void update();
	ULLVec3 rayTrace(Ray ray);
};