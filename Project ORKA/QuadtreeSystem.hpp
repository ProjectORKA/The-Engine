#pragma once

#include "Debug.hpp"
#include "QuadtreeID.hpp"
#include "TerrainSystem.hpp"
#include "Settings.hpp"

struct QuadtreeNode;

struct QuadtreeData {
	Terrain terrain;
};

struct QuadtreeNode {

	QuadtreeData data;
	
	ULL users = 0; //number of chunks possibly using this as their terrain

	//octree stuff
	Bool subdivided = false;

	QuadtreeID id;

	QuadtreeNode * parent = nullptr;
	SharedPointer<QuadtreeNode> c00 = nullptr;
	SharedPointer<QuadtreeNode> c01 = nullptr;
	SharedPointer<QuadtreeNode> c10 = nullptr;
	SharedPointer<QuadtreeNode> c11 = nullptr;

	void count();
	void update();
	void destroy();
	void subdivide();
	void unsubdivide();
	void incrementUsers();
	void decrementUsers();
	void createRootNode();
	void createChildNode(QuadtreeNode & parent, Bool x, Bool y);
};