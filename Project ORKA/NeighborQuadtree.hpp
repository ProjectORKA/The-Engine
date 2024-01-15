#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"


struct Renderer;

struct NeighborQuadtreeNode
{
	NeighborQuadtreeNode* parent = nullptr;

	NeighborQuadtreeNode* c00 = nullptr;
	NeighborQuadtreeNode* c01 = nullptr;
	NeighborQuadtreeNode* c10 = nullptr;
	NeighborQuadtreeNode* c11 = nullptr;

	NeighborQuadtreeNode* nl = nullptr;
	NeighborQuadtreeNode* nr = nullptr;
	NeighborQuadtreeNode* nb = nullptr;
	NeighborQuadtreeNode* nf = nullptr;

	Byte level      = 0;
	Bool subdivided = false;
	Vec3 position   = Vec3(0);
	Vec4 color      = Vec4(randomVec3Fast(), 1);

	[[nodiscard]] NeighborQuadtreeNode& nlr() const;
	[[nodiscard]] NeighborQuadtreeNode& nrr() const;
	[[nodiscard]] NeighborQuadtreeNode& nbr() const;
	[[nodiscard]] NeighborQuadtreeNode& nfr() const;

	void subdivide();
	void unSubdivide();
	void update(Vec3 location);
	void removeSelfFromNeighbors() const;
	void create(NeighborQuadtreeNode& parent, Bool x, Bool y);
	void render(Renderer& renderer) const;
};

struct NeighborQuadtree
{
	NeighborQuadtreeNode root;
	void                 create();
	void                 destroy();
	void                 update(Vec3 location);
	void                 render(Renderer& renderer) const;
};
