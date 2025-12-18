#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"

inline void proceduralWireframeAxisLines(CpuMesh& cpuMesh)
{
	cpuMesh.primitiveMode      = PrimitiveMode::Lines;
	cpuMesh.positions          = Vec3Vector({Vec3(0.0f, 0.0f, 0.0f), Vec3(1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.textureCoordinates = Vec2Vector({Vec2(0.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.vertexNormals      = Vec3Vector({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors       = Vec3Vector({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices            = Vector<Index>({0, 1, 0, 2, 0, 3});
	cpuMesh.checkIntegrity();
}

inline void proceduralPlaneMesh(CpuMesh& cpuMesh, const Float width, const Float height)
{
	cpuMesh.positions          = Vec3Vector({Vec3(+width, +height, 0.0f), Vec3(+width, -height, 0.0f), Vec3(-width, +height, 0.0f), Vec3(-width, -height, 0.0f)});
	cpuMesh.textureCoordinates = Vec2Vector({Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.vertexNormals      = Vec3Vector({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors       = Vec3Vector({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices            = Vector<Index>({3, 0, 2, 3, 1, 0});
	cpuMesh.checkIntegrity();
}

// Quad mesh with flipped V to render letter atlas upright in UI space
inline void proceduralLetterQuadMesh(CpuMesh& cpuMesh, const Float width, const Float height)
{
	cpuMesh.positions          = Vec3Vector({Vec3(+width, +height, 0.0f), Vec3(+width, -height, 0.0f), Vec3(-width, +height, 0.0f), Vec3(-width, -height, 0.0f)});
	cpuMesh.textureCoordinates = Vec2Vector({Vec2(1.0f, 0.0f), Vec2(1.0f, 1.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 1.0f)});
	cpuMesh.vertexNormals      = Vec3Vector({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors       = Vec3Vector({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices            = Vector<Index>({3, 0, 2, 3, 1, 0});
	cpuMesh.checkIntegrity();
}

inline void proceduralWireframeCubeMesh(CpuMesh& cpuMesh, const Float size, const Float size2)
{
	cpuMesh.primitiveMode      = PrimitiveMode::Lines;
	cpuMesh.positions          = Vec3Vector({Vec3(size, size, 1), Vec3(size, size, 0), Vec3(size, size2, 1), Vec3(size, size2, 0), Vec3(size2, size, 1), Vec3(size2, size, 0), Vec3(size2, size2, 1), Vec3(size2, size2, 0)});
	cpuMesh.textureCoordinates = Vec2Vector({Vec2(1.0f, 1.0f), Vec2(1.0f, 1.0f), Vec2(1.0f, 0.0f), Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 1.0f), Vec2(0.0f, 0.0f), Vec2(0.0f, 0.0f)});
	cpuMesh.vertexNormals      = Vec3Vector({Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, 1.0f)});
	cpuMesh.vertexColors       = Vec3Vector({Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f), Vec3(1.0f, 1.0f, 1.0f)});
	cpuMesh.indices            = Vector<Index>({7, 3, 7, 5, 7, 6, 1, 3, 1, 5, 1, 0, 2, 6, 2, 0, 2, 3, 4, 6, 4, 5, 4, 0});
	cpuMesh.checkIntegrity();
}

inline void proceduralTriangleStripGrid(CpuMesh& cpuMesh, const Int xSegments, const Int ySegments, const Float width, const Float height)
{
	const Float dx          = (2.0f * width) / static_cast<Float>(xSegments);
	const Float dy          = (2.0f * height) / static_cast<Float>(ySegments);
	const Int   vertexCount = (xSegments + 1) * (ySegments + 1);
	cpuMesh.positions.reserve(vertexCount);
	cpuMesh.textureCoordinates.reserve(vertexCount);
	cpuMesh.vertexNormals.reserve(vertexCount);
	cpuMesh.vertexColors.reserve(vertexCount);
	for (Int i = 0; i <= ySegments; ++i)
	{
		for (Int j = 0; j <= xSegments; ++j)
		{
			const Float x = -width + j * dx;
			const Float y = -height + i * dy;
			cpuMesh.positions.push_back(Vec3(x, y, 0.0f));
			const Float u = static_cast<Float>(j) / static_cast<Float>(xSegments);
			const Float v = static_cast<Float>(i) / static_cast<Float>(ySegments);
			cpuMesh.textureCoordinates.push_back(Vec2(u, v));
			cpuMesh.vertexNormals.push_back(Vec3(0.0f, 0.0f, 1.0f));
			cpuMesh.vertexColors.push_back(Vec3(1.0f, 1.0f, 1.0f));
		}
	}
	cpuMesh.primitiveMode = PrimitiveMode::TriangleStrip;
	{
		const Int rowSize   = 2 * (xSegments + 1) + 2;
		const Int totalSize = ySegments * rowSize - 2;
		cpuMesh.indices.reserve(totalSize);
	}
	for (Int row = 0; row < ySegments; ++row)
	{
		if (row > 0)
		{
			const Int lastRowEnd = (row * (xSegments + 1)) + xSegments;
			const Int nextRowEnd = ((row + 1) * (xSegments + 1)) + xSegments;
			cpuMesh.indices.push_back(lastRowEnd);
			cpuMesh.indices.push_back(nextRowEnd);
		}
		for (Int col = 0; col <= xSegments; ++col)
		{
			const Int top    = (row + 1) * (xSegments + 1) + col;
			const Int bottom = row * (xSegments + 1) + col;
			cpuMesh.indices.push_back(top);
			cpuMesh.indices.push_back(bottom);
		}
	}
	cpuMesh.checkIntegrity();
}
