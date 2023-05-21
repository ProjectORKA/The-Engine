#pragma once

#include "GPUMesh.hpp"

struct Renderer;

inline CpuMesh convertLineToMesh(const Line3D& line);
inline CpuMesh convertLinesToMesh(const Vector<Line3D>& lines);

struct LineRenderer {
	GpuMesh gpuMesh;
	CpuMesh cpuMesh;

	void create();
	void renderLine(Renderer& renderer, Vec2 start, Vec2 end, Float width);
	void renderLine(Renderer& renderer, Vec3 start, Vec3 end, Float width);
	void renderLines(Renderer& renderer, const Vector<Line3D>& lines);
};
