#pragma once

#include "Math.hpp"
#include "Basics.hpp"
#include "Debug.hpp"
#include "QuadtreeSystem.hpp"
#include "OctreeID.hpp"
#include "OctreeData.hpp"
#include "Settings.hpp"

#define ONE_METER_CHUNK_LEVEL 39					//at this level 1 unit = 1 meter
#define CHUNK_USAGE_TICK_COUNT 180					//amount of ticks that a chunk will stay active before it will be deleted (needs to be updated regularly)

struct OctreeNode {

	OctreeNodeData data;

	OctreeID id;

	Bool isValid = false;

	//octree stuff
	Mutex mutex;
	UInt users = 0;
	OctreeNode* parentNode = nullptr;
	bool subdivided = false;
	OctreeNode* c000 = nullptr;
	OctreeNode* c001 = nullptr;
	OctreeNode* c010 = nullptr;
	OctreeNode* c011 = nullptr;
	OctreeNode* c100 = nullptr;
	OctreeNode* c101 = nullptr;
	OctreeNode* c110 = nullptr;
	OctreeNode* c111 = nullptr;

	std::shared_ptr<QuadtreeNode> quadtreeEquivalent = nullptr;

	ULLVec3 getLocation();

	void count();
	void update();
	void destroy();
	void subdivide();
	void unsubdivide();
	void incrementUser();
	void decrementUser();
	void createRootNode(std::shared_ptr<QuadtreeNode> quadTreeRoot);
	void createChildNode(OctreeNode* parent, Bool x, Bool y, Bool z);
};