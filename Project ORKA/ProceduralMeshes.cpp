#include "ProceduralMeshes.hpp"

void proceduralWireframeCubeMesh(CPUMesh& cpuMesh, Float size)
{
	cpuMesh.primitiveMode = Lines;

	cpuMesh.vertices = {
		Vec3(size,  size,  size),
		Vec3(size,  size, -size),
		Vec3(size, -size,  size),
		Vec3(size, -size, -size),
		Vec3(-size,  size,  size),
		Vec3(-size,  size, -size),
		Vec3(-size, -size,  size),
		Vec3(-size, -size, -size)
	};

	cpuMesh.uvs = {
		Vec2(1.0, 1.0),
		Vec2(1.0, 1.0),
		Vec2(1.0, 0.0),
		Vec2(1.0, 0.0),
		Vec2(0.0, 1.0),
		Vec2(0.0, 1.0),
		Vec2(0.0, 0.0),
		Vec2(0.0, 0.0)
	};

	cpuMesh.indices = {
		7, 3, 7,
		5, 7, 6,
		1, 3, 1,
		5, 1, 0,
		2, 6, 2,
		0, 2, 3,
		4, 6, 4,
		5, 4, 0
	};

	cpuMesh.readyForUpload = true;
}
void proceduralPlaneMesh(CPUMesh& cpuMesh, Float width, Float height) {

	cpuMesh.vertices =
	{
		Vec3( width,  height, 0.0f),
		Vec3( width, -height, 0.0f),
		Vec3(-width,  height, 0.0f),
		Vec3(-width, -height, 0.0f)
	};

	cpuMesh.uvs = {
		Vec2(1.0f, 1.0f),
		Vec2(1.0f, 0.0f),
		Vec2(0.0f, 1.0f),
		Vec2(0.0f, 0.0f)
	};

	cpuMesh.indices = {
		3, 0, 2,
		3, 1, 0
	};

	cpuMesh.readyForUpload = true;
};