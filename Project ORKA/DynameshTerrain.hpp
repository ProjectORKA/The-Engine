
#pragma once

#include "Basics.hpp"
#include "Engine.hpp"

struct Renderer;
struct Camera;

struct DynameshNode {
	Bool exists = true;

	DynameshNode* parent = nullptr;

	Vec3 position = Vec3(1);
	Float size = 1.0f;

	~DynameshNode() {
		exists = false;
	}

	void render(Engine& engine, Renderer& renderer);
};

struct DynameshTerrain {
	Int lastIndex = 0;
	Vector<DynameshNode> nodes;
	Bool loaded = false;

	void update(Camera camera);
	void render(Engine& engine, Renderer& renderer);
};