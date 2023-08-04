#pragma once

#include "OctreeNode.hpp"

struct QuadtreeSystem;

struct OctreeSystem
{
	OctreeNode* root = nullptr;

	void destroy();
	void count() const;
	void update() const;
	void create(const QuadtreeSystem& quadTreeSystem);
};
