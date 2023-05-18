#pragma once

#include "Basics.hpp"
#include "CpuMesh.hpp"
#include "GPUMesh.hpp"

struct Renderer;

struct Plot {
	Vector<Vec2> points;

	void add(Vec2 value);
	CpuMesh createMesh() const;
	void add(Float a, Float b);
	void render(Renderer& renderer) const;
	void render(Renderer& renderer, Float lineWidth) const;
};
