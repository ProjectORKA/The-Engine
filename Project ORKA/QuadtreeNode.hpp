
#pragma once

#include "QuadtreeData.hpp"
#include "Ray.hpp"

struct QuadtreeNode {
	QuadtreeData data;
	
	ULL users = 0; //number of active octree nodes related to this quadtree node
	Bool subdivided = false;
	QuadtreeID id;
	//parent
	QuadtreeNode* parent = nullptr;
	//children
	QuadtreeNode* c00 = nullptr;
	QuadtreeNode* c01 = nullptr;
	QuadtreeNode* c10 = nullptr;
	QuadtreeNode* c11 = nullptr;

	~QuadtreeNode();
	QuadtreeNode();
	QuadtreeNode(QuadtreeNode* parent, Bool x, Bool y);

	void count();
	void update();
	void subdivide();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
};