#pragma once
#include "Renderer.hpp"

constexpr Int quadtreeDistance = 1024;

struct SimpleRtsSimulation;

struct SimpleRtsQuadtreeNode
{
	Int  id         = 0;
	Int  x          = 0;
	Int  y          = 0;
	Int  level      = 0;
	Int  parent     = -1;
	Bool subdivided = false;

	void subdivide(SimpleRtsSimulation& sim);
	void update(SimpleRtsSimulation& sim, Float mapSize);
	SimpleRtsQuadtreeNode(SimpleRtsSimulation& sim, Int x, Int y, Int level, Int parent, Int id);
};

struct SimpleRtsQuadtreeSystem
{
	Vector<Vector<SimpleRtsQuadtreeNode>> nodes;
	const Int                             levelCount = 10;
	Vector<Matrix>                        treeLevelMatrices;
	Vector<Matrix>                        quadtreeNodeMatrices;

	void render(Renderer& r) const;
	void create(SimpleRtsSimulation & sim);
	void update(SimpleRtsSimulation& sim, Float mapSize);
};
