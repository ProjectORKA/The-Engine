#pragma once

#include "OctreeData.hpp"
#include "OctreeId.hpp"
#include "Basics.hpp"
#include "QuadtreeNode.hpp"

struct OcTreeNode {
	OctreeNodeData data;
	OctreeId id;
	Bool isValid = false;
	Mutex mutex;
	UInt users = 0;
	Bool subdivided = false;
	//parent node
	OcTreeNode* parent = nullptr;
	//child nodes
	OcTreeNode* c000 = nullptr;
	OcTreeNode* c001 = nullptr;
	OcTreeNode* c010 = nullptr;
	OcTreeNode* c011 = nullptr;
	OcTreeNode* c100 = nullptr;
	OcTreeNode* c101 = nullptr;
	OcTreeNode* c110 = nullptr;
	OcTreeNode* c111 = nullptr;
	//look at 2d structure
	QuadtreeNode* quadTreeEquivalent = nullptr;

	~OcTreeNode();
	explicit OcTreeNode(QuadtreeNode& quadTreeRoot);
	OcTreeNode(OcTreeNode& parent, Bool x, Bool y, Bool z);

	void count() const;
	void update();
	void subdivide();
	void unSubdivide();
	void incrementUser();
	void decrementUser();
	ULLVec3 getLocation() const;
};
