#pragma once

#include "Basics.hpp"
#include "ResourceManager.hpp"
#include "Transform.hpp"

struct Renderer;
struct NeighborOctreeNode;

struct NeighborOctreeNode
{
	NeighborOctreeNode* parent = nullptr;

	NeighborOctreeNode* c000 = nullptr;
	NeighborOctreeNode* c001 = nullptr;
	NeighborOctreeNode* c010 = nullptr;
	NeighborOctreeNode* c011 = nullptr;
	NeighborOctreeNode* c100 = nullptr;
	NeighborOctreeNode* c101 = nullptr;
	NeighborOctreeNode* c110 = nullptr;
	NeighborOctreeNode* c111 = nullptr;

	NeighborOctreeNode* nl = nullptr;
	NeighborOctreeNode* nr = nullptr;
	NeighborOctreeNode* nb = nullptr;
	NeighborOctreeNode* nf = nullptr;
	NeighborOctreeNode* nt = nullptr;
	NeighborOctreeNode* nd = nullptr;

	[[nodiscard]] NeighborOctreeNode& nlr() const;
	[[nodiscard]] NeighborOctreeNode& nrr() const;
	[[nodiscard]] NeighborOctreeNode& nbr() const;
	[[nodiscard]] NeighborOctreeNode& nfr() const;
	[[nodiscard]] NeighborOctreeNode& ntr() const;
	[[nodiscard]] NeighborOctreeNode& ndr() const;

	Byte level            = 0;
	Bool inRenderDistance = false;
	Bool isTerrain        = false;
	Bool isSurface        = false;
	Bool subdivided       = false;
	Bool hasAllNeighbors  = false;
	Vec3 position         = Vec3(0);

	void subdivide();
	void unSubdivide();
	void updateIsSurface();
	void update(Vec3 pos);
	void updateHasAllNeighbors();
	void removeSelfFromNeighbors() const;
	void create(NeighborOctreeNode& parent, Bool x, Bool y, Bool z);
};

struct NeighborOctree
{
	NeighborOctreeNode root;
	void               create();
	void               destroy();
	void               update(Vec3 position);
};

void renderNeighborOctreeNode(const NeighborOctreeNode& node, Renderer& renderer);
