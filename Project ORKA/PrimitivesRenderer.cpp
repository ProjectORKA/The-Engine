#include "PrimitivesRenderer.hpp"

void PrimitivesRenderer::create()
{
	CpuMesh genMesh;

	// points
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "PointCloud";
	genMesh.primitiveMode = PrimitiveMode::Points;
	genMesh.positions     = {Vec3(0.0f, 0.0f, 0.0f)};
	genMesh.checkIntegrity();
	pointsMesh.upload(genMesh);
	genMesh.clearGeometry();

	// line
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "Line";
	genMesh.primitiveMode = PrimitiveMode::LineStrip;
	genMesh.positions     = {Vec3(-0.5f, 0.0f, 0.0f), Vec3(0.5f, 0.0f, 0.0f)};
	genMesh.indices       = {0, 1};
	genMesh.checkIntegrity();
	lineMesh.upload(genMesh);
	genMesh.clearGeometry();

	// lines
	genMesh.drawMode      = BufferUsage::DynamicDraw;
	genMesh.name          = "Lines";
	genMesh.primitiveMode = PrimitiveMode::Lines;
	genMesh.positions     = {Vec3(-0.5f, 0.0f, 0.0f), Vec3(0.5f, 0.0f, 0.0f)};
	genMesh.indices       = {0, 1};
	genMesh.checkIntegrity();
	linesMesh.upload(genMesh);
	genMesh.clearGeometry();

	// plane
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "Plane";
	genMesh.primitiveMode      = PrimitiveMode::Triangles;
	genMesh.positions          = {Vec3(-0.5f, -0.5f, 0), Vec3(0.5f, -0.5f, 0), Vec3(-0.5f, 0.5f, 0), Vec3(0.5f, 0.5f, 0)};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(0, 1), Vec2(1, 1)};
	genMesh.indices            = {0, 1, 2, 2, 1, 3};
	genMesh.checkIntegrity();
	planeMesh.upload(genMesh);
	genMesh.clearGeometry();

	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "Rectangle";
	genMesh.primitiveMode      = PrimitiveMode::Triangles;
	genMesh.positions          = {Vec3(0.0f, 0.0f, 0), Vec3(1.0f, 0.0f, 0), Vec3(0.0f, 1.0f, 0), Vec3(1.0f, 1.0f, 0)};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(0, 1), Vec2(1, 1)};
	genMesh.indices            = {0, 1, 2, 2, 1, 3};
	genMesh.checkIntegrity();
	rectangleMesh.upload(genMesh);
	genMesh.clearGeometry();

	// wireframe cube
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "WireframeCube";
	genMesh.primitiveMode      = PrimitiveMode::Lines;
	genMesh.positions          = {Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(1, 1, 0), Vec3(0, 1, 0), Vec3(0, 0, 1), Vec3(1, 0, 1), Vec3(1, 1, 1), Vec3(0, 1, 1),};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1),};
	genMesh.indices            = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
	genMesh.checkIntegrity();
	wireframeCubeMesh.upload(genMesh);
	genMesh.clearGeometry();

	// wireframe cube centered
	genMesh.drawMode           = BufferUsage::StaticDraw;
	genMesh.name               = "WireframeCubeCentered";
	genMesh.primitiveMode      = PrimitiveMode::Lines;
	genMesh.positions          = {Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f), Vec3(-0.5f, -0.5f, 0.5f), Vec3(0.5f, -0.5f, 0.5f), Vec3(0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f),};
	genMesh.textureCoordinates = {Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1), Vec2(0, 0), Vec2(1, 0), Vec2(1, 1), Vec2(0, 1),};
	genMesh.indices            = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
	genMesh.checkIntegrity();
	wireframeCubeCenteredMesh.upload(genMesh);
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
	circleMesh.upload(genMesh);
	genMesh.clearGeometry();
}

void PrimitivesRenderer::destroy()
{
	lineMesh.unload();
	cubeMesh.unload();
	linesMesh.unload();
	planeMesh.unload();
	pointsMesh.unload();
	circleMesh.unload();
	rectangleMesh.unload();
	wireframeCubeMesh.unload();
	wireframeCubeCenteredMesh.unload();
}

void PrimitivesRenderer::rectangle(Uniforms& uniforms) const
{
	rectangleMesh.render(uniforms);
}

void PrimitivesRenderer::plane(Uniforms& uniforms) const
{
	planeMesh.render(uniforms);
}

void PrimitivesRenderer::wireframeCube(Uniforms& uniforms) const
{
	wireframeCubeMesh.render(uniforms);
}

void PrimitivesRenderer::wireframeCubeCentered(Uniforms& uniforms) const
{
	wireframeCubeCenteredMesh.render(uniforms);
}

void PrimitivesRenderer::points(const Vec3Vector& pos, Uniforms& uniforms)
{
	Vector<Index> indices;
	for (int i = 0; i < pos.size(); i++) indices.emplace_back(i);
	pointsMesh.updatePositionBuffer(pos);
	pointsMesh.updateIndexBuffer(indices);
	pointsMesh.render(uniforms);
}

void PrimitivesRenderer::points(const Vec2Vector& pos, Uniforms& uniforms)
{
	points(vec2VectorToVec3Vector(pos), uniforms);
}

void PrimitivesRenderer::line(const Vec2Vector& positions, Uniforms& uniforms)
{
	line(vec2VectorToVec3Vector(positions), uniforms);
}

void PrimitivesRenderer::line(const Vec3Vector& positions, Uniforms& uniforms)
{
	Vector<Index> indices;
	for (int i = 0; i < positions.size(); i++) indices.emplace_back(i);
	lineMesh.updatePositionBuffer(positions);
	lineMesh.updateIndexBuffer(indices);
	lineMesh.render(uniforms);
}

void PrimitivesRenderer::lines(const Vec2Vector& positions, Uniforms& uniforms)
{
	lines(vec2VectorToVec3Vector(positions), uniforms);
}

void PrimitivesRenderer::lines(const Vec3Vector& positions, Uniforms& uniforms)
{
	Vector<Index> indices;
	for (int i = 0; i < positions.size(); i++) indices.emplace_back(i);
	lines(positions, indices, uniforms);
}

void PrimitivesRenderer::circles(const Vector<Matrix>& matrices, Uniforms& uniforms) const
{
	circleMesh.renderInstances(uniforms, matrices);
}

void PrimitivesRenderer::wireframeCubes(const Vector<Matrix>& matrices, Uniforms& uniforms) const
{
	wireframeCubeMesh.renderInstances(uniforms, matrices);
}

void PrimitivesRenderer::lines(const Vec3Vector& positions, const Vector<Index>& indices, Uniforms& uniforms)
{
	linesMesh.updatePositionBuffer(positions);
	linesMesh.updateIndexBuffer(indices);
	linesMesh.render(uniforms);
}
