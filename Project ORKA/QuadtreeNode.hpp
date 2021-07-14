
#pragma once

#include "QuadtreeData.hpp"

struct QuadtreeNode {

	QuadtreeData data;

	ULL users = 0; //number of chunks possibly using this as their terrain

	//octree stuff
	Bool subdivided = false;

	QuadtreeID id;

	//parent
	QuadtreeNode* parent = nullptr;
	//children
	QuadtreeNode* c00 = nullptr;
	QuadtreeNode* c01 = nullptr;
	QuadtreeNode* c10 = nullptr;
	QuadtreeNode* c11 = nullptr;
	//neighbours
	QuadtreeNode* n00 = nullptr;
	QuadtreeNode* n01 = nullptr;
	QuadtreeNode* n10 = nullptr;
	QuadtreeNode* n11 = nullptr;

	void count();
	void update();
	void destroy();
	void subdivide();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
	void createRootNode();
	void createChildNode(QuadtreeNode* parent, Bool x, Bool y);
};