#pragma once

#include "Basics.hpp"
#include "CpuMesh.hpp"

struct Renderer;

struct BranchStructure {
	Vector<Index> connections;
	void render(Renderer& renderer, const Vector<Vec3>& points) const;
};
