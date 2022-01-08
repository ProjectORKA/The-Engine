
#pragma once

#include "Basics.hpp"

struct Renderer;
struct Camera;

struct DynameshNode {
	DynameshNode* parent = nullptr;

	Vec3 position = Vec3(1);
	Float size = 1.0f;

	void render(Renderer& renderer);
};

struct DynameshTerrain {
	Int lastIndex = 0;
	Vector<DynameshNode> nodes;

	void create();
	void update(Camera camera);
	void render(Renderer& renderer);
};