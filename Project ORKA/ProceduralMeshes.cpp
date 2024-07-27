#include "ProceduralMeshes.hpp"

void proceduralWireframeAxisLines(CpuMesh& cpuMesh)
{
	cpuMesh.primitiveMode = PrimitiveMode::Lines;
	cpuMesh.positions = Vector<Vec3>({Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.textureCoordinates = Vector<Vec2>({Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.normals = Vector<Vec3>({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors = Vector<Vec3>({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices = Vector<Index>({0, 1, 0, 2, 0, 3});
	cpuMesh.checkIntegrity();
}

void proceduralPlaneMesh(CpuMesh& cpuMesh, const Float width, const Float height)
{
	cpuMesh.positions = Vector<Vec3>({Vec3(+width, +height, 0.0f), Vec3(+width, -height, 0.0f), Vec3(-width, +height, 0.0f), Vec3(-width, -height, 0.0f)});
	cpuMesh.textureCoordinates = Vector<Vec2>({Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.normals = Vector<Vec3>({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors = Vector<Vec3>({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices = Vector<Index>({3, 0, 2, 3, 1, 0});
	cpuMesh.checkIntegrity();
}

void proceduralWireframeCubeMesh(CpuMesh& cpuMesh, const Float size, const Float size2)
{
	cpuMesh.primitiveMode = PrimitiveMode::Lines;
	cpuMesh.positions = Vector<Vec3>({Vec3(size, size, 1), Vec3(size, size, 0), Vec3(size, size2, 1), Vec3(size, size2, 0), Vec3(size2, size, 1), Vec3(size2, size, 0), Vec3(size2, size2, 1), Vec3(size2, size2, 0)});
	cpuMesh.textureCoordinates = Vector<Vec2>({Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.normals = Vector<Vec3>({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors = Vector<Vec3>({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices = Vector<Index>({7, 3, 7, 5, 7, 6, 1, 3, 1, 5, 1, 0, 2, 6, 2, 0, 2, 3, 4, 6, 4, 5, 4, 0});
	cpuMesh.checkIntegrity();
}
