#pragma once
#include "GPUMesh.hpp"

struct PrimitivesRenderer
{
	// [TODO]: check if pointsMesh is rendered using indices or if they can be removed
private:
	GpuMesh lineMesh;
	GpuMesh linesMesh;
	GpuMesh cubeMesh;
	GpuMesh planeMesh;
	GpuMesh pointsMesh;
	GpuMesh wireframeCubeMesh;
	GpuMesh wireframeCubeCenteredMesh;

public:
	void create();
	void destroy();
	void rectangle(Uniforms& uniforms) const;
	void wireframeCube(Uniforms& uniforms) const;
	void wireframeCubeCentered(Uniforms& uniforms) const;
	void points(const Vector<Vec3>& pos, Uniforms& uniforms);
	void points(const Vector<Vec2>& pos, Uniforms& uniforms);
	void line(const Vector<Vec2>& positions, Uniforms& uniforms);
	void line(const Vector<Vec3>& positions, Uniforms& uniforms);
	void lines(const Vector<Vec2>& positions, Uniforms& uniforms);
	void lines(const Vector<Vec3>& positions, Uniforms& uniforms);
	void wireframeCubes(Uniforms& uniforms, const Vector<Matrix>& matrices) const;
	void lines(const Vector<Vec3>& positions, const Vector<Index>& indices, Uniforms& uniforms);
};
