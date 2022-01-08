
#pragma once

#include "Basics.hpp"

struct Renderer;
struct CPUMesh;

struct PointCloud {
	Vector<Vec3> points;

	void add(Vec3 point);
	CPUMesh createMesh();
	void render(Renderer& renderer);
};