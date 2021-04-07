#include "ProceduralMeshes.hpp"

void proceduralWireframeAxisLines(CPUMesh& cpuMesh)
{
	cpuMesh.primitiveMode = Lines;

	cpuMesh.vertices = {
		Vec3(0,0,0),
		Vec3(1,0,0),
		Vec3(0,1,0),
		Vec3(0,0,1)
	};

	cpuMesh.uvs = {
		Vec2(0.0, 0.0),
		Vec2(1.0, 0.0),
		Vec2(0.0, 1.0),
		Vec2(0.0, 0.0)
	};

	cpuMesh.normals = {
		Vec3(0,0,1),
		Vec3(0,0,1),
		Vec3(0,0,1),
		Vec3(0,0,1)
	};

	cpuMesh.indices = {
		0, 1, 0, 2 , 0 , 3
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

	cpuMesh.normals = {
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f)
	};

	cpuMesh.indices = {
		3, 0, 2,
		3, 1, 0
	};

	cpuMesh.readyForUpload = true;
};
void proceduralWireframeCubeMesh(CPUMesh& cpuMesh, Float size, Float size2)
{
	cpuMesh.primitiveMode = Lines;

	cpuMesh.vertices = {
		Vec3(size,size,1),
		Vec3(size,size,0),
		Vec3(size,size2,1),
		Vec3(size,size2,0),
		Vec3(size2,size,1),
		Vec3(size2,size,0),
		Vec3(size2,size2,1),
		Vec3(size2,size2,0)
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

	cpuMesh.normals = {
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f),
	Vec3(0.0f, 0.0f, 1.0f)
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