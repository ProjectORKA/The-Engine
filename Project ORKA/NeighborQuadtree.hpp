
#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"

struct Renderer;

struct NeighborQuadtreeNode {
	NeighborQuadtreeNode* parent = nullptr;
	
	NeighborQuadtreeNode* c00 = nullptr;
	NeighborQuadtreeNode* c01 = nullptr;
	NeighborQuadtreeNode* c10 = nullptr;
	NeighborQuadtreeNode* c11 = nullptr;

	NeighborQuadtreeNode* nl = nullptr;
	NeighborQuadtreeNode* nr = nullptr;
	NeighborQuadtreeNode* nb = nullptr;
	NeighborQuadtreeNode* nf = nullptr;

	UChar level = 0;
	Vec3 position = Vec3(0);
	Boolean subdivided = false;
	Vec4 color = Vec4(randomVec3(), 1);
	
	NeighborQuadtreeNode& nlr();
	NeighborQuadtreeNode& nrr();
	NeighborQuadtreeNode& nbr();
	NeighborQuadtreeNode& nfr();

	void subdivide();//
	void unsubdivide();//
	void update(Vec3 location);
	void render(Renderer& renderer);
	void removeSelfFromNeighbours();//
	void create(NeighborQuadtreeNode& parent, Bool x, Bool y);//
};

struct NeighborQuadtree {
	NeighborQuadtreeNode root;
	void create();
	void destroy();
	void update(Vec3 location);
	void render(Renderer& renderer);
};