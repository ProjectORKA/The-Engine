#pragma once

#include "Basics.hpp"

struct QuadtreeNode {
	Index ne = -1;
	Index nw = -1;
	Index se = -1;
	Index sw = -1;
};
template<UInt count>
struct Quadtree {
	Index nextFree = 0;
	Vector<QuadtreeNode> nodes[count];
	Quadtree() {
		setupIndices();
	}
	void setupIndices() {
		for (Index i = 0; i < count; i++) {
			nodes[i].ne = i+1;
		}
		nodes[count] = -1;
	}
	
};