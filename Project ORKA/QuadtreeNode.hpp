
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
	QuadtreeNode* nl = nullptr;
	QuadtreeNode* nr = nullptr;
	QuadtreeNode* nb = nullptr;
	QuadtreeNode* nf = nullptr;

	QuadtreeNode& nlr();
	QuadtreeNode& nrr();
	QuadtreeNode& nbr();
	QuadtreeNode& nfr();

	void count();
	void destroy();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
	void removeSelfFromNeighbours();
	void create(TerrainSystem& terrainSystem);
	void subdivide(TerrainSystem& terrainSystem);
	QuadtreeNode & get(TerrainSystem& terrainSystem, QuadtreeID id);
	void update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem, QuadtreeNode* parent, Bool x, Bool y);
};