#include "PointRenderer.hpp"
#include "Renderer.hpp"

void PointRenderer::create()
{
	cpuMesh.drawMode      = BufferUsage::DynamicDraw;
	cpuMesh.name          = "PointCloud";
	cpuMesh.primitiveMode = PrimitiveMode::Points;
}

void PointRenderer::destroy()
{
	gpuMesh.unload();
}

void PointRenderer::update(const Vector<Vec3>& points)
{
	if(gpuMesh.isLoaded()) gpuMesh.unload();
	for(Index i = 0; i < points.size(); i++) cpuMesh.indices.push_back(i);
	cpuMesh.positions = points;
	cpuMesh.checkIntegrity();
	gpuMesh.upload(cpuMesh);
}

void PointRenderer::render(Renderer& renderer, const Vector<Vec3>& points)
{
	update(points);
	gpuMesh.render(renderer.uniforms());
}

void PointRenderer::render(Renderer& renderer, const Vector<Vec2>& points)
{
	render(renderer, vec2VectorToVec3Vector(points));
}
