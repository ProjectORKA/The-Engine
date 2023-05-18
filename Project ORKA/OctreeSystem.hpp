#pragma once

#include "OcTreeNode.hpp"

struct QuadTreeSystem;

struct OcTreeSystem {
	OcTreeNode* root = nullptr;

	void count() const;
	void update() const;
	void destroy();
	void create(const QuadTreeSystem& quadTreeSystem);
};
