
#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"

struct Plot {
	Vector<Vec2> points;

	void render();
	void add(Vec2 value);
	CPUMesh createMesh();
	void add(Float a, Float b);
};