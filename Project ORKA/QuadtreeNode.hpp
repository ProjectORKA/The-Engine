#pragma once

#include "QuadtreeID.hpp"
#include "QuadtreeData.hpp"
#include "Ray.hpp"

struct QuadTreeSystem;

struct QuadtreeNode {
	QuadtreeData data;

	ULL users = 0; //number of active octree nodes related to this quadtree node
	Bool subdivided = false;
	UShort index = -1;
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

	void count() const;
	void destroy();
	void unsubdivide();
	void decrementUsers();
	void incrementUsers();
	void removeSelfFromNeighbours() const;
	void subdivide(TerrainSystem& terrainSystem);
	void createRootNode(TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, const QuadtreeID& id);
	void update(QuadTreeSystem& quadtreeSystem, TerrainSystem& terrainSystem);
	void create(TerrainSystem& terrainSystem, QuadtreeNode* parent, Bool x, Bool y);
};
