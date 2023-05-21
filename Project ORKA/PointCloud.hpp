#pragma once

#include "Basics.hpp"
#include "GPUMesh.hpp"

struct Renderer;
struct CpuMesh;

struct PointCloud {
	Vector<Vec3> points;
	void add(Vec3 point);
};

struct PointCloudRenderer {
	GpuMesh gpuMesh;
	UInt pointCloudSize = 0;
	void update(PointCloud& pointCloud);
	void render(PointCloud& pointCloud, Renderer& renderer);
};
