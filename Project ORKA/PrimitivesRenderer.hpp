#pragma once
#include "GPUMesh.hpp"

struct PrimitivesRenderer
{
	// [TODO]: check if pointsMesh is rendered using indices or if they can be removed
private:
	GpuMesh lineMesh;
	GpuMesh cubeMesh;
	GpuMesh linesMesh;
	GpuMesh planeMesh;
	GpuMesh pointsMesh;
	GpuMesh circleMesh;
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
	void circles(const Vector<Matrix>& matrices, Uniforms& uniforms) const;
	void wireframeCubes(const Vector<Matrix>& matrices, Uniforms& uniforms) const;
	void lines(const Vector<Vec3>& positions, const Vector<Index>& indices, Uniforms& uniforms);
};
