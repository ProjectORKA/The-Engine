#include "LineRenderer.hpp"
#include "Renderer.hpp"

void LineRenderer::create()
{
	cpuMesh.drawMode = BufferUsage::DynamicDraw;
	cpuMesh.indices.push_back(0);
	cpuMesh.indices.push_back(1);
	cpuMesh.indices.push_back(2);
	cpuMesh.indices.push_back(3);
	cpuMesh.name = "line";
	cpuMesh.normals.push_back(Vec3(0, 0, 1));
	cpuMesh.normals.push_back(Vec3(0, 0, 1));
	cpuMesh.normals.push_back(Vec3(0, 0, 1));
	cpuMesh.normals.push_back(Vec3(0, 0, 1));
	cpuMesh.primitiveMode = PrimitiveMode::TriangleStrip;
	cpuMesh.textureCoordinates.push_back(Vec2(0, 1));
	cpuMesh.textureCoordinates.push_back(Vec2(0, 0));
	cpuMesh.textureCoordinates.push_back(Vec2(1, 0));
	cpuMesh.textureCoordinates.push_back(Vec2(1, 1));
	cpuMesh.vertexColors.push_back(Vec3(1));
	cpuMesh.vertexColors.push_back(Vec3(1));
	cpuMesh.vertexColors.push_back(Vec3(1));
	cpuMesh.vertexColors.push_back(Vec3(1));
	cpuMesh.positions.resize(4);
}

void LineRenderer::destroy()
{
	gpuMesh.unload();
}

CPUMesh convertLineToMesh(const Line3D& line)
{
	CPUMesh mesh;
	mesh.name          = "Line";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	mesh.indices.push_back(0);
	mesh.indices.push_back(1);
	mesh.positions.push_back(line.start);
	mesh.positions.push_back(line.end);
	mesh.checkIntegrity();
	return mesh;
}

CPUMesh convertLineToMesh(const Vector<Vec2>& line)
{
	CPUMesh mesh;
	mesh.name          = "Line";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for(Index i = 0; i < line.size() - 1; i++)
	{
		mesh.indices.push_back(i);
		mesh.indices.push_back(i + 1);
		mesh.positions.push_back(Vec3(line[i], 0));
	}
	mesh.positions.push_back(Vec3(line.back(), 0));
	mesh.checkIntegrity();
	return mesh;
}

CPUMesh convertLineToMesh(const Vector<Vec3>& line)
{
	CPUMesh mesh;
	mesh.name          = "Line";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for(Index i = 0; i < line.size() - 1; i++)
	{
		mesh.indices.push_back(i);
		mesh.indices.push_back(i + 1);
		mesh.positions.push_back(line[i]);
	}
	mesh.positions.push_back(line.back());
	mesh.checkIntegrity();
	return mesh;
}

CPUMesh convertLinesToMesh(const Vector<Vec3>& lines)
{
	CPUMesh mesh;
	mesh.name          = "Lines";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for(Index i = 0; i < lines.size(); i++)
	{
		mesh.indices.push_back(i);
		mesh.positions.push_back(lines[i]);
	}
	mesh.checkIntegrity();
	return mesh;
}

CPUMesh convertLinesToMesh(const Vector<Vec2>& lines)
{
	CPUMesh mesh;
	mesh.name          = "Lines";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for(Index i = 0; i < lines.size(); i++)
	{
		mesh.indices.push_back(i);
		mesh.positions.push_back(Vec3(lines[i], 0));
	}
	mesh.checkIntegrity();
	return mesh;
}

CPUMesh convertLinesToMesh(const Vector<Line3D>& lines)
{
	CPUMesh mesh;
	mesh.name          = "Lines";
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for(Index i = 0; i < lines.size(); i++)
	{
		mesh.indices.push_back(i * 2);
		mesh.indices.push_back(i * 2 + 1);
		mesh.positions.push_back(lines[i].start);
		mesh.positions.push_back(lines[i].end);
	}
	mesh.checkIntegrity();
	return mesh;
}

void LineRenderer::renderLine(Renderer& renderer, const Vector<Vec2>& line)
{
	if(line.size() >= 2)
	{
		renderer.uniforms().setMMatrix(Matrix(1));
		cpuMesh = convertLineToMesh(line);
		gpuMesh.upload(cpuMesh);
		gpuMesh.render(renderer.uniforms());
		gpuMesh.unload();
	}
}

void LineRenderer::renderLine(Renderer& renderer, const Vector<Vec3>& line)
{
	if(line.size() >= 2)
	{
		renderer.uniforms().setMMatrix(Matrix(1));
		cpuMesh = convertLineToMesh(line);
		gpuMesh.upload(cpuMesh);
		gpuMesh.render(renderer.uniforms());
		gpuMesh.unload();
	}
}

void LineRenderer::renderLines(Renderer& renderer, const Vector<Vec2>& lines)
{
	if(lines.size() >= 2)
	{
		renderer.uniforms().setMMatrix(Matrix(1));
		cpuMesh = convertLinesToMesh(lines);
		gpuMesh.upload(cpuMesh);
		gpuMesh.render(renderer.uniforms());
		gpuMesh.unload();
	}
}

void LineRenderer::renderLines(Renderer& renderer, const Vector<Vec3>& lines)
{
	if(lines.size() >= 2)
	{
		renderer.uniforms().setMMatrix(Matrix(1));
		cpuMesh = convertLinesToMesh(lines);
		gpuMesh.upload(cpuMesh);
		gpuMesh.render(renderer.uniforms());
		gpuMesh.unload();
	}
}

void LineRenderer::renderLines(Renderer& renderer, const Vector<Line3D>& lines)
{
	if(!lines.empty())
	{
		renderer.uniforms().setMMatrix(Matrix(1));
		cpuMesh = convertLinesToMesh(lines);
		gpuMesh.upload(cpuMesh);
		gpuMesh.render(renderer.uniforms());
		gpuMesh.unload();
	}
}

void LineRenderer::renderLine(Renderer& renderer, const Vec2 start, const Vec2 end)
{
	Vector<Vec3> line(2);
	line[0] = Vec3(start, 0);
	line[1] = Vec3(end, 0);
	renderLine(renderer, line);
}

void LineRenderer::renderLine(Renderer& renderer, const Vec3 start, const Vec3 end)
{
	Vector<Vec3> line(2);
	line[0] = start;
	line[1] = end;
	renderLine(renderer, line);
}

void LineRenderer::renderLineAdvanced(Renderer& renderer, const Vec2 start, const Vec2 end, const Float width)
{
	renderLineAdvanced(renderer, Vec3(start, 0), Vec3(end, 0), width);
}

void LineRenderer::renderLineAdvanced(Renderer& renderer, const Vec3 start, const Vec3 end, const Float width)
{
	renderer.uniforms().setMMatrix(Matrix(1));
	const Vec3 dir       = normalize(start - end);
	const Vec3 extend    = cross(dir, Vec3(0, 0, 1));
	cpuMesh.positions[0] = start + extend * width;
	cpuMesh.positions[1] = start - extend * width;
	cpuMesh.positions[2] = end + extend * width;
	cpuMesh.positions[3] = end - extend * width;

	cpuMesh.checkIntegrity();
	gpuMesh.upload(cpuMesh);
	gpuMesh.render(renderer.uniforms());
	gpuMesh.unload();
}
