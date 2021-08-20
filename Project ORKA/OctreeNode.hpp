
#pragma once

#include "Random.hpp"
#include "OctreeData.hpp"
#include "OctreeID.hpp"
#include "QuadtreeNode.hpp"

struct OctreeSystem;

struct OctreeNode {

	OctreeNodeData data;
	OctreeID id;
	Bool isValid = false;
	Mutex mutex;
	UInt users = 0;
	bool subdivided = false;
	//parent node
	OctreeNode* parent = nullptr;
	//child nodes
	OctreeNode* c000 = nullptr;
	OctreeNode* c001 = nullptr;
	OctreeNode* c010 = nullptr;
	OctreeNode* c011 = nullptr;
	OctreeNode* c100 = nullptr;
	OctreeNode* c101 = nullptr;
	OctreeNode* c110 = nullptr;
	OctreeNode* c111 = nullptr;
	//look at 2d structure
	QuadtreeNode* quadtreeEquivalent = nullptr;

	~OctreeNode();
	OctreeNode(QuadtreeNode& quadTreeRoot);
	OctreeNode(OctreeNode& parent, Bool x, Bool y, Bool z);

	void count();
	void update();
	void subdivide();
	void unsubdivide();
	void incrementUser();
	void decrementUser();
	ULLVec3 getLocation();
};