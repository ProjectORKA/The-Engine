#pragma once

#include "GPUMesh.hpp"

struct Renderer;

struct LineRenderer {

	GPUMesh gpuMesh;
	CPUMesh cpuMesh;

	void create();
	void renderLine(Renderer& renderer, Vec3 start, Vec3 end, Float width);
};