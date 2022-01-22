
#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"

struct Renderer;

struct BranchStructure {
	Vector<Index> connections;
	void render(Renderer& renderer, Vector<Vec3> points);
};