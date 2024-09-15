#include "PointRenderer.hpp"
#include "Renderer.hpp"

//void PointRenderer::create()
//{
//	cpuMesh.drawMode      = BufferUsage::DynamicDraw;
//	cpuMesh.name          = "PointCloud";
//	cpuMesh.primitiveMode = PrimitiveMode::Points;
//}

//void PointRenderer::destroy()
//{
//	gpuMesh.unload();
//}

//void PointRenderer::update(const Vec3Vector& points)
//{
//	if(gpuMesh.isLoaded()) gpuMesh.unload();
//	cpuMesh.indices.clear();
//	for(Index i = 0; i < points.size(); i++) cpuMesh.indices.push_back(i);
//	cpuMesh.positions = points;
//	cpuMesh.checkIntegrity();
//	gpuMesh.upload(cpuMesh);
//}

//void PointRenderer::render(Renderer& renderer, const Vec3Vector& points)
//{
//	update(points);
//	gpuMesh.render(renderer.uniforms());
//}
//
//void PointRenderer::render(Renderer& renderer, const Vec2Vector& points)
//{
//	render(renderer, vec2VectorToVec3Vector(points));
//}
