#pragma once

#include "QuadtreeNode.hpp"

struct QuadtreeSystem {
	QuadtreeNode root;

	void count();
	void create();
	void destroy();
	void update();
};