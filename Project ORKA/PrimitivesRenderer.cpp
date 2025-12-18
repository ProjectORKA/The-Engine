#include "PrimitivesRenderer.hpp"

void PrimitivesRenderer::create(Renderer & renderer)
{
	CpuMesh genMesh;

	// points
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "PointCloud";
	genMesh.primitiveMode = PrimitiveMode::Points;
	genMesh.positions     = {Vec3(0.0f, 0.0f, 0.0f)};
	genMesh.checkIntegrity();
	pointsMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// line
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "Line";
	genMesh.primitiveMode = PrimitiveMode::LineStrip;
	genMesh.positions     = {Vec3(-0.5f, 0.0f, 0.0f), Vec3(0.5f, 0.0f, 0.0f)};
	genMesh.indices       = {0, 1};
	genMesh.checkIntegrity();
	lineMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// lines
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "Lines";
	genMesh.primitiveMode = PrimitiveMode::Lines;
	genMesh.positions     = {Vec3(-0.5f, 0.0f, 0.0f), Vec3(0.5f, 0.0f, 0.0f)};
	genMesh.indices       = {0, 1};
	genMesh.checkIntegrity();
	linesMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// plane
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "Plane";
	genMesh.primitiveMode      = PrimitiveMode::Triangles;
	genMesh.positions          = {Vec3(-0.5f, -0.5f, 0), Vec3(0.5f, -0.5f, 0), Vec3(-0.5f, 0.5f, 0), Vec3(0.5f, 0.5f, 0)};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(0, 1), Vec2(1, 1)};
	genMesh.indices            = {0, 1, 2, 2, 1, 3};
	genMesh.checkIntegrity();
	planeMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "Rectangle";
	genMesh.primitiveMode      = PrimitiveMode::Triangles;
	genMesh.positions          = {Vec3(0.0f, 0.0f, 0), Vec3(1.0f, 0.0f, 0), Vec3(0.0f, -1.0f, 0), Vec3(1.0f, -1.0f, 0)};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(0, 1), Vec2(1, 1)};
	genMesh.indices            = {0, 1, 2, 2, 1, 3};
	genMesh.checkIntegrity();
	rectangleMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// wireframe cube
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "WireframeCube";
	genMesh.primitiveMode      = PrimitiveMode::Lines;
	genMesh.positions          = {Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(1, 1, 0), Vec3(0, 1, 0), Vec3(0, 0, 1), Vec3(1, 0, 1), Vec3(1, 1, 1), Vec3(0, 1, 1),};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1),};
	genMesh.indices            = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
	genMesh.checkIntegrity();
	wireframeCubeMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// wireframe cube centered
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "WireframeCubeCentered";
	genMesh.primitiveMode      = PrimitiveMode::Lines;
	genMesh.positions          = {Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f), Vec3(-0.5f, -0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f),};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1),};
	genMesh.indices            = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
	genMesh.checkIntegrity();
	wireframeCubeCenteredMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();

	// circle
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "Circle";
	genMesh.primitiveMode      = PrimitiveMode::TriangleFan;
	Int circleResolution = 6;
	genMesh.positions          = circleOfPoints(0.5f, circleResolution);
	genMesh.positions.emplace_back(0,0,0);
	
	genMesh.textureCoordinates = vec3VectorToVec2Vector(genMesh.positions);

	genMesh.indices            = {6,0,1,2,3,4,5,0};
	genMesh.checkIntegrity();
	circleMesh.upload(renderer, genMesh);
	genMesh.clearGeometry();
}

void PrimitivesRenderer::destroy(Renderer & renderer)
{
	lineMesh.unload(renderer);
	cubeMesh.unload(renderer);
	linesMesh.unload(renderer);
	planeMesh.unload(renderer);
	pointsMesh.unload(renderer);
	circleMesh.unload(renderer);
	rectangleMesh.unload(renderer);
	wireframeCubeMesh.unload(renderer);
	wireframeCubeCenteredMesh.unload(renderer);
}

void PrimitivesRenderer::rectangle(Renderer& renderer) const
{
	rectangleMesh.render(renderer);
}

void PrimitivesRenderer::plane(Renderer& renderer) const
{
	planeMesh.render(renderer);
}

void PrimitivesRenderer::wireframeCube(Renderer& renderer) const
{
	wireframeCubeMesh.render(renderer);
}

void PrimitivesRenderer::wireframeCubeCentered(Renderer& renderer) const
{
	wireframeCubeCenteredMesh.render(renderer);
}

void PrimitivesRenderer::points(Renderer& renderer, const Vec3Vector& pos)
{
	Vector<Index> indices;
	for (int i = 0; i < pos.size(); i++) indices.emplace_back(i);
	pointsMesh.updatePositionBuffer(renderer, pos);
	pointsMesh.updateIndexBuffer(renderer, indices);
	pointsMesh.render(renderer);
}

void PrimitivesRenderer::points(Renderer& renderer, const Vec2Vector& pos)
{
	points(renderer, vec2VectorToVec3Vector(pos));
}

void PrimitivesRenderer::line(Renderer& renderer, const Vec2Vector& positions)
{
	line(renderer, vec2VectorToVec3Vector(positions));
}

void PrimitivesRenderer::line(Renderer& renderer, const Vec3Vector& positions)
{
	Vector<Index> indices;
	for (int i = 0; i < positions.size(); i++) indices.emplace_back(i);
	lineMesh.updatePositionBuffer(renderer, positions);
	lineMesh.updateIndexBuffer(renderer, indices);
	lineMesh.render(renderer);
}

void PrimitivesRenderer::lines(Renderer& renderer, const Vec2Vector& positions)
{
	lines(renderer, vec2VectorToVec3Vector(positions));
}

void PrimitivesRenderer::lines(Renderer& renderer, const Vec3Vector& positions)
{
	Vector<Index> indices;
	for (int i = 0; i < positions.size(); i++) indices.emplace_back(i);
	lines(renderer, positions, indices);
}

void PrimitivesRenderer::circles(Renderer& renderer, const Vector<Matrix>& matrices)
{
	circleMesh.renderInstances(renderer, matrices);
}

void PrimitivesRenderer::wireframeCubes(Renderer& renderer, const Vector<Matrix>& matrices)
{
	wireframeCubeMesh.renderInstances(renderer, matrices);
}

void PrimitivesRenderer::lines(Renderer& renderer, const Vec3Vector& positions, const Vector<Index>& indices)
{
	linesMesh.updatePositionBuffer(renderer, positions);
	linesMesh.updateIndexBuffer(renderer, indices);
	linesMesh.render(renderer);
}
