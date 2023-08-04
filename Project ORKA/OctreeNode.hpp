#pragma once

#include "OctreeId.hpp"
#include "Basics.hpp"
#include "QuadtreeNode.hpp"

struct OctreeNode
{
	OctreeId id;
	Mutex    mutex;
	UInt     users      = 0;
	Bool     isValid    = false;
	Bool     subdivided = false;
	// parent node
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
	// look at 2d structure
	QuadtreeNode* quadTreeEquivalent = nullptr;

	~OctreeNode();
	explicit OctreeNode(QuadtreeNode& quadTreeRoot);
	OctreeNode(OctreeNode& parent, Bool x, Bool y, Bool z);

	void                  update();
	void                  subdivide();
	void                  unSubdivide();
	void                  count() const;
	void                  incrementUser();
	void                  decrementUser();
	[[nodiscard]] ULLVec3 getLocation() const;
};
