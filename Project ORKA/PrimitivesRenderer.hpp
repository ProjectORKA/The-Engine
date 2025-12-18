#pragma once
#include "GPUMesh.hpp"

struct PrimitivesRenderer
{
	// [TODO]: check if pointsMesh is rendered using indices or if they can be removed
private:
	GPUMesh lineMesh;
	GPUMesh uiSquare;
	GPUMesh cubeMesh;
	GPUMesh linesMesh;
	GPUMesh planeMesh;
	GPUMesh pointsMesh;
	GPUMesh circleMesh;
	GPUMesh rectangleMesh;
	GPUMesh wireframeCubeMesh;
	GPUMesh wireframeCubeCenteredMesh;

public:
	void create(Renderer & renderer);
	void destroy(Renderer & renderer);
	void plane(Renderer & renderer) const;
	void rectangle(Renderer & renderer) const;
	void wireframeCube(Renderer & renderer) const;
	void wireframeCubeCentered(Renderer & renderer) const;
	void points(Renderer & renderer, const Vec3Vector& pos);
	void points(Renderer & renderer, const Vec2Vector& pos);
	void line(Renderer & renderer, const Vec2Vector& positions);
	void line(Renderer & renderer, const Vec3Vector& positions);
	void lines(Renderer & renderer, const Vec2Vector& positions);
	void lines(Renderer & renderer, const Vec3Vector& positions);
	void circles(Renderer & renderer, const Vector<Matrix>& matrices);
	void wireframeCubes(Renderer & renderer, const Vector<Matrix>& matrices);
	void lines(Renderer & renderer, const Vec3Vector& positions, const Vector<Index>& indices);
};
