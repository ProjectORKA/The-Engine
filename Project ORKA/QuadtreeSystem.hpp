#pragma once

#include "QuadtreeNode.hpp"

struct QuadtreeSystem {
	QuadtreeNode * root = nullptr;

	void count();
	void create();
	void update();
	void destroy();
	//void getAllChunksInRelativeRange(ULLVec3 location, Float relativeRange) {
	//	List<QuadtreeNode*> checkThis;
	//	List<QuadtreeNode*> result;

	//	checkThis.push_back(root);

	//	for (QuadtreeNode* checkedNode : checkThis) {
	//		checkedNode->id.location 
	//	}
	//}


	ULLVec3 rayTrace(Ray ray);
	QuadtreeNode& get(QuadtreeID id);
};