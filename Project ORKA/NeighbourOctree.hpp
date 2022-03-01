
#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"

#include "PointCloud.hpp"

struct Renderer;

struct NeighbourOctreeNode;

struct NeighbourOctreeNodeData {
	PointCloudRenderer pcr;
	PointCloud points;
	//Vec4 color = Vec4(randomVec3(), 1);
	
	void render(Renderer& renderer);
	void create(NeighbourOctreeNode& node);
	void update(NeighbourOctreeNode& node);
	void destroy() {
		
	}
};

struct NeighbourOctreeNode {
	
	NeighbourOctreeNodeData data;
	
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
	Bool rendered = false;
	Vec3 position = Vec3(0);
	Boolean subdivided = false;

	void subdivide();
	void unsubdivide();
	void update(Vec3 location);
	void render(Renderer& renderer);
	void removeSelfFromNeighbours();
	void create(NeighbourOctreeNode& parent, Bool x, Bool y, Bool z);
};

struct NeighbourOctree {
	NeighbourOctreeNode root;
	void create();
	void destroy();
	void update(Vec3 location);
	void render(Renderer& renderer);
};