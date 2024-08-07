#pragma once

#include "GPUMesh.hpp"

struct Renderer;

inline CpuMesh convertLineToMesh(const Line3D& line);
inline CpuMesh convertLineToMesh(const Vector<Vec2>& line);
inline CpuMesh convertLineToMesh(const Vector<Vec3>& line);
inline CpuMesh createMeshFromLines(const Vector<Vec2>& lines);
inline CpuMesh createMeshFromLines(const Vector<Vec3>& lines);
inline CpuMesh createMeshFromLines(const Vector<Line3D>& lines);

//struct LineRenderer
//{
//	//GPUMesh gpuMesh;
//	//CPUMesh cpuMesh;
//
//	//void create();
//	//void destroy();
//	//void centeredCube();
//	//void renderLine(Renderer& renderer, Vec2 start, Vec2 end);
//	//void renderLine(Renderer& renderer, Vec3 start, Vec3 end);
//	//void renderLine(Renderer& renderer, const Vector<Vec3>& line);
//	//void renderLines(Renderer& renderer, const Vector<Vec2>& lines);
//	//void renderLines(Renderer& renderer, const Vector<Vec3>& lines);
//	//void renderLines(Renderer& renderer, const Vector<Line3D>& lines);
//	//void renderLineAdvanced(Renderer& renderer, Vec2 start, Vec2 end, Float width);
//	//void renderLineAdvanced(Renderer& renderer, Vec3 start, Vec3 end, Float width);
//	//void renderLine(Renderer& renderer, const Vector<Vec2>& line, const Matrix& matrix);
//	//void renderLines(Renderer& renderer, const Vector<Vec2>& lines, const Matrix& matrix);
//};
