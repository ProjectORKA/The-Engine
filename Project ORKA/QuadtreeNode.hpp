#pragma once

#include "QuadtreeId.hpp"
#include "QuadtreeData.hpp"

struct QuadtreeSystem;

struct QuadtreeNode
{
	QuadtreeId   id;
	QuadtreeData data;
	ULL          users      = 0; // number of active octree nodes related to this quadtree node
	UShort       index      = -1;
	Bool         subdivided = false;

	QuadtreeNode* parent = nullptr;
	QuadtreeNode* c00    = nullptr;
	QuadtreeNode* c01    = nullptr;
	QuadtreeNode* c10    = nullptr;
	QuadtreeNode* c11    = nullptr;
	QuadtreeNode* nl     = nullptr;
	QuadtreeNode* nr     = nullptr;
	QuadtreeNode* nb     = nullptr;
	QuadtreeNode* nf     = nullptr;

	[[nodiscard]] QuadtreeNode& nlr() const;
	[[nodiscard]] QuadtreeNode& nrr() const;
	[[nodiscard]] QuadtreeNode& nbr() const;
	[[nodiscard]] QuadtreeNode& nfr() const;

	void          destroy();
	void          unsubdivide();
	void          count() const;
	void          decrementUsers();
	void          incrementUsers();
	void          removeSelfFromNeighbors() const;
	void          subdivide(const TerrainSystem& terrainSystem);
	void          createRootNode(const TerrainSystem& terrainSystem);
	QuadtreeNode& get(TerrainSystem& terrainSystem, const QuadtreeId& id);
	void          update(QuadtreeSystem& quadtreeSystem, TerrainSystem& terrainSystem);
	void          create(const TerrainSystem& terrainSystem, QuadtreeNode* parent, Bool x, Bool y);
};
