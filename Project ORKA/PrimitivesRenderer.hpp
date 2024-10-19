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
	GpuMesh rectangleMesh;
	GpuMesh wireframeCubeMesh;
	GpuMesh wireframeCubeCenteredMesh;

public:
	void create(Renderer & renderer);
	void destroy(Renderer & renderer);
	void plane(Uniforms& uniforms) const;
	void rectangle(Uniforms& uniforms) const;
	void wireframeCube(Uniforms& uniforms) const;
	void wireframeCubeCentered(Uniforms& uniforms) const;
	void points(const Vec3Vector& pos, Uniforms& uniforms);
	void points(const Vec2Vector& pos, Uniforms& uniforms);
	void line(const Vec2Vector& positions, Uniforms& uniforms);
	void line(const Vec3Vector& positions, Uniforms& uniforms);
	void lines(const Vec2Vector& positions, Uniforms& uniforms);
	void lines(const Vec3Vector& positions, Uniforms& uniforms);
	void circles(const Vector<Matrix>& matrices, Uniforms& uniforms) const;
	void wireframeCubes(const Vector<Matrix>& matrices, Uniforms& uniforms) const;
	void lines(const Vec3Vector& positions, const Vector<Index>& indices, Uniforms& uniforms);
};
