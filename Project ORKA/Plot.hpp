
#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"

struct Renderer;

struct Plot {
	Vector<Vec2> points;

	void add(Vec2 value);
	CPUMesh createMesh();
	void add(Float a, Float b);
	void render(Renderer& renderer);
	void render(Renderer& renderer, Float lineWidth);
};