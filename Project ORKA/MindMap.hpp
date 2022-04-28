#pragma once

#include "Random.hpp"

struct Renderer;

struct MindmapConnection {
	Index a;
	Index b;

	MindmapConnection(Index a, Index b) {
		this->a = a;
		this->b = b;
	};
};

struct MindMap {

	UInt nodeCount = 0;

	Vector<Vec2> positions;
	Vector<Vec2> forces;
	Vector<Float> numforces;

	Vector<MindmapConnection> connections;

	void update();
	void addNode();
	void render(Renderer& renderer);
	void renderInteractive(Renderer& renderer);
	void addForce(Index a, Vec2 force);
};