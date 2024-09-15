#pragma once

#include "Basics.hpp"


struct Renderer;

struct LeveledGraphConnection
{
	Index a;
	Index b;

	LeveledGraphConnection(Index a, Index b);
};

struct LeveledGraph
{
	Vec2Vector                   forces;
	Vector<Float>                  numForces;
	Vec2Vector                   positions;
	Vector<LeveledGraphConnection> connections;
	UInt                           nodeCount = 0;

	void update();
	void addNode();
	Vec3 getPos(Index x, Index y);
	void addForce(Index a, Vec2 force);
	Vec3 getPos(Index x, Index y, Float z);
	void render(Renderer& renderer) const;
};
