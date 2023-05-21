#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"
#include "ResourceManager.hpp"

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

	Byte level = 0;
	Vec3 position = Vec3(0);
	Bool subdivided = false;
	Vec4 color = Vec4(randomVec3(), 1);

	NeighborQuadtreeNode& nlr() const;
	NeighborQuadtreeNode& nrr() const;
	NeighborQuadtreeNode& nbr() const;
	NeighborQuadtreeNode& nfr() const;

	void subdivide();
	void unSubdivide();
	void update(Vec3 location);
	void removeSelfFromNeighbors() const;
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
	void create(NeighborQuadtreeNode& parent, Bool x, Bool y);
};

struct NeighborQuadtree
{
	NeighborQuadtreeNode root;
	void create();
	void destroy();
	void update(Vec3 location);
	void render(ResourceManager& resourceManager, Renderer& renderer);
};