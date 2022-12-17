#pragma once

#include "QuadtreeID.hpp"
#include "QuadtreeData.hpp"
#include "Ray.hpp"

struct QuadtreeSystem;

struct QuadtreeNode {
	QuadtreeData data;

	ULL users = 0; //number of active octree nodes related to this quadtree node
	Bool subdivided = false;
	QuadtreeID id;

	QuadtreeNode* parent = nullptr;
	QuadtreeNode* c00 = nullptr;
	QuadtreeNode* c01 = nullptr;
	QuadtreeNode* c10 = nullptr;
	QuadtreeNode* c11 = nullptr;
	QuadtreeNode* nn = nullptr;
	QuadtreeNode* ns = nullptr;
	QuadtreeNode* ne = nullptr;
	QuadtreeNode* nw = nullptr;

	//get available neighbour
	QuadtreeNode& nnr();
	QuadtreeNode& nsr();
	QuadtreeNode& ner();
	QuadtreeNode& nwr();

	//get available diagonal neighbour
	QuadtreeNode& nner();
	QuadtreeNode& nnwr();
	QuadtreeNode& nser();
	QuadtreeNode& nswr();

	void count();
	void destroy();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
	void removeSelfFromNeighbours();
	void subdivide(TerrainSystem& terrainSystem);
	void createRootNode(TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, QuadtreeID id);
	void update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem, QuadtreeNode* parent, Bool x, Bool y);
};

struct QuadtreeSystem {
	QuadtreeNode * root = nullptr;

	void count();
	ULLVec3 rayTrace(Ray ray);
	void update(TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, QuadtreeID id);
};