#pragma once

#include "Debug.hpp"
#include "Basics.hpp"
#include "QuadtreeID.hpp"
#include "QuadtreeData.hpp"
#include "Velox_Vector.hpp"

struct QuadtreeNode2 {
	QuadtreeData data;

	ULL users = 0; //number of active octree nodes related to this quadtree node
	Bool subdivided = false;
	QuadtreeID id;

	QuadtreeNode2* parent = nullptr;
	QuadtreeNode2* c00 = nullptr;
	QuadtreeNode2* c01 = nullptr;
	QuadtreeNode2* c10 = nullptr;
	QuadtreeNode2* c11 = nullptr;
	QuadtreeNode2* nn = nullptr;
	QuadtreeNode2* ns = nullptr;
	QuadtreeNode2* ne = nullptr;
	QuadtreeNode2* nw = nullptr;

	//get available neighbour
	QuadtreeNode2& nnr();
	QuadtreeNode2& nsr();
	QuadtreeNode2& ner();
	QuadtreeNode2& nwr();

	//get available diagonal neighbour
	QuadtreeNode2& nner();
	QuadtreeNode2& nnwr();
	QuadtreeNode2& nser();
	QuadtreeNode2& nswr();

	void count();
	void destroy();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
	void removeSelfFromNeighbours();
	void subdivide(TerrainSystem& terrainSystem);
	void createRootNode(TerrainSystem& terrainSystem);
	QuadtreeNode2& get(TerrainSystem& terrainSystem, QuadtreeID id);
	void update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem, QuadtreeNode2* parent, Bool x, Bool y);
};

struct QuadtreeSystem2 {
	TRE::Vector<QuadtreeNode2> levels[MAX_CHUNK_LEVEL];
	TRE::Vector<QuadtreeNode2> activeNodes;


	TerrainSystem terrainSystem;

	void clear() {
		for (int i = 0; i < MAX_CHUNK_LEVEL; i++) {
			levels[i].Clear();
		}
	}
	QuadtreeSystem2() {
		clear();
		add(0).createRootNode(terrainSystem);
	}
	QuadtreeNode2& add(Int level) {
		return levels[level].EmplaceBack();
	}

	void createNode(QuadtreeNode2& node, Bool x, Bool y, QuadtreeNode2& parent, TerrainSystem& terrainSystem) {
		node.id.level = parent.id.level + 1;
	}

	void subdivide(QuadtreeNode2& node) {
		if ((!node.subdivided) && (node.id.level < MAX_CHUNK_LEVEL - 1)) {
			//get level
			UShort childLevel = node.id.level;
			QuadtreeNode2& c00 = levels[childLevel].EmplaceBack();
			QuadtreeNode2& c01 = levels[childLevel].EmplaceBack();
			QuadtreeNode2& c10 = levels[childLevel].EmplaceBack();
			QuadtreeNode2& c11 = levels[childLevel].EmplaceBack();

			//create children
			createNode(c00, 0, 0, node, terrainSystem);
			createNode(c01, 0, 1, node, terrainSystem);
			createNode(c10, 1, 0, node, terrainSystem);
			createNode(c11, 1, 1, node, terrainSystem);

			//mark as subdivided
			node.subdivided = true;
		}
	}

	void count() {
		ULL count = 0;
		for (int i = 0; i < MAX_CHUNK_LEVEL; i++) {
			count += levels[i].Size();
		}
		logDebug(count);
	}
};