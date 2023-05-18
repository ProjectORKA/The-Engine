#pragma once

#include "Basics.hpp"
#include "Random.hpp"
#include "ResourceManager.hpp"

struct Renderer;

struct LevelledGraphConnection {
	Index a;
	Index b;

	LevelledGraphConnection(const Index a, const Index b) {
		this->a = a;
		this->b = b;
	};
};

struct LevelledGraph {
	UInt nodeCount = 0;

	Vector<Vec2> positions;
	Vector<Vec2> forces;
	Vector<Float> numforces;

	Vector<LevelledGraphConnection> connections;

	void update();
	void addNode();
	Vec3 getPos(Index x, Index y);
	void addForce(Index a, Vec2 force);
	Vec3 getPos(Index x, Index y, Float z);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
};
