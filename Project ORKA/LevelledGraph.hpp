#pragma once

#include "Basics.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct LeveledGraphConnection
{
	Index a;
	Index b;

	LeveledGraphConnection(Index a, Index b);
};

struct LeveledGraph
{
	Vector<Vec2>                   forces;
	Vector<Float>                  numForces;
	Vector<Vec2>                   positions;
	Vector<LeveledGraphConnection> connections;
	UInt                           nodeCount = 0;

	void update();
	void addNode();
	Vec3 getPos(Index x, Index y);
	void addForce(Index a, Vec2 force);
	Vec3 getPos(Index x, Index y, Float z);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};
