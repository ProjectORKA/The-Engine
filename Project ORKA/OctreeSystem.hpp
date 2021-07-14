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

struct OctreeNode;

struct OctreeSystem;

struct OctreeNode {

	OctreeNodeData data;

	OctreeID id;

	Bool isValid = false;

	//octree stuff
	Mutex mutex;
	UInt users = 0;
	bool subdivided = false;
	
	//parent node
	OctreeNode * parent = nullptr;
	//child nodes
	OctreeNode * c000 = nullptr;
	OctreeNode * c001 = nullptr;
	OctreeNode * c010 = nullptr;
	OctreeNode * c011 = nullptr;
	OctreeNode * c100 = nullptr;
	OctreeNode * c101 = nullptr;
	OctreeNode * c110 = nullptr;
	OctreeNode * c111 = nullptr;

	//look at 2d structure
	QuadtreeNode * quadtreeEquivalent = nullptr;

	ULLVec3 getLocation();

	void count();
	void destroy();
	void unsubdivide();
	void incrementUser();
	void decrementUser();
	void update();
	void subdivide();
	void createRootNode(QuadtreeNode & quadTreeRoot);
	void createChildNode(OctreeNode & parent, Bool x, Bool y, Bool z);
};

struct OctreeSystem {
	OctreeNode root;

	void count();
	void create(QuadtreeSystem& quadtreeSystem);;
	void destroy();
	void update();
	
	OctreeNode& get(OctreeID octreeID);
};