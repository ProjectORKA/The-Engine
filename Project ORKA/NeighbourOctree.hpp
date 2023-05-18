#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"
#include "PointCloud.hpp"
#include "Threading.hpp"

struct Renderer;
struct NeighbourOctreeNode;

struct NeighbourOctreeNode {
	NeighbourOctreeNode* parent = nullptr;

	NeighbourOctreeNode* c000 = nullptr;
	NeighbourOctreeNode* c001 = nullptr;
	NeighbourOctreeNode* c010 = nullptr;
	NeighbourOctreeNode* c011 = nullptr;
	NeighbourOctreeNode* c100 = nullptr;
	NeighbourOctreeNode* c101 = nullptr;
	NeighbourOctreeNode* c110 = nullptr;
	NeighbourOctreeNode* c111 = nullptr;

	NeighbourOctreeNode* nl = nullptr;
	NeighbourOctreeNode* nr = nullptr;
	NeighbourOctreeNode* nb = nullptr;
	NeighbourOctreeNode* nf = nullptr;
	NeighbourOctreeNode* nt = nullptr;
	NeighbourOctreeNode* nd = nullptr;

	NeighbourOctreeNode& nlr();
	NeighbourOctreeNode& nrr();
	NeighbourOctreeNode& nbr();
	NeighbourOctreeNode& nfr();
	NeighbourOctreeNode& ntr();
	NeighbourOctreeNode& ndr();

	UChar level = 0;
	Bool inRenderDistance = false;
	Bool isTerrain = false;
	Bool isSurface = false;
	Vec3 position = Vec3(0);
	Bool subdivided = false;
	Bool hasAllNeighbours = false;

	void subdivide();
	void unsubdivide();
	void updateIsSurface();
	void update(Vec3 location);
	void updateHasAllNeighbours();
	void removeSelfFromNeighbours() const;
	void create(NeighbourOctreeNode& parent, Bool x, Bool y, Bool z);
};

struct NeighbourOctree {
	NeighbourOctreeNode root;
	void create();
	void destroy();
	void update(Vec3 location);
};

void renderNeighbourOctreeNode(ResourceManager& resourceManager, const NeighbourOctreeNode& node, Renderer& renderer);
