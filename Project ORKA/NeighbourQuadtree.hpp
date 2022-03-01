
#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "Transform.hpp"

struct Renderer;

struct NeighbourQuadtreeNode {
	NeighbourQuadtreeNode* parent = nullptr;
	
	NeighbourQuadtreeNode* c00 = nullptr;
	NeighbourQuadtreeNode* c01 = nullptr;
	NeighbourQuadtreeNode* c10 = nullptr;
	NeighbourQuadtreeNode* c11 = nullptr;

	NeighbourQuadtreeNode* nl = nullptr;
	NeighbourQuadtreeNode* nr = nullptr;
	NeighbourQuadtreeNode* nb = nullptr;
	NeighbourQuadtreeNode* nf = nullptr;

	UChar level = 0;
	Vec3 position = Vec3(0);
	Boolean subdivided = false;
	Vec4 color = Vec4(randomVec3(), 1);
	
	NeighbourQuadtreeNode& nlr();
	NeighbourQuadtreeNode& nrr();
	NeighbourQuadtreeNode& nbr();
	NeighbourQuadtreeNode& nfr();

	void subdivide();//
	void unsubdivide();//
	void update(Vec3 location);
	void render(Renderer& renderer);
	void removeSelfFromNeighbours();//
	void create(NeighbourQuadtreeNode& parent, Bool x, Bool y);//
};

struct NeighbourQuadtree {
	NeighbourQuadtreeNode root;
	void create();
	void destroy();
	void update(Vec3 location);
	void render(Renderer& renderer);
};