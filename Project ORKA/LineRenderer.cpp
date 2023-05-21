#include "LineRenderer.hpp"
#include "Renderer.hpp"

void LineRenderer::create() {
	cpuMesh.drawMode = MeshDrawMode::DynamicMode;
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

void LineRenderer::renderLine(Renderer& renderer, const Vec3 start, const Vec3 end, const Float width) {
	renderer.uniforms().mMatrix(Matrix(1));
	const Vec3 dir = normalize(start - end);
	const Vec3 extend = cross(dir, Vec3(0, 0, 1));
	cpuMesh.positions[0] = (start + extend * width);
	cpuMesh.positions[1] = (start - extend * width);
	cpuMesh.positions[2] = (end + extend * width);
	cpuMesh.positions[3] = (end - extend * width);

	cpuMesh.checkIntegrity();
	gpuMesh.upload(cpuMesh);
	gpuMesh.render(renderer.uniforms());
	gpuMesh.unload();
}

void LineRenderer::renderLines(Renderer& renderer, const Vector<Line3D>& lines) {
	renderer.uniforms().mMatrix(Matrix(1));
	cpuMesh = convertLinesToMesh(lines);
	gpuMesh.upload(cpuMesh);
	gpuMesh.render(renderer.uniforms());
	gpuMesh.unload();
}

CpuMesh convertLineToMesh(const Line3D& line) {
	CpuMesh mesh;
	mesh.name = "Line";
	mesh.dataFlags = Positions;
	mesh.drawMode = MeshDrawMode::DynamicMode;
	mesh.primitiveMode = PrimitiveMode::Lines;
	mesh.indices.push_back(0);
	mesh.indices.push_back(1);
	mesh.positions.push_back(line.start);
	mesh.positions.push_back(line.end);
	mesh.checkIntegrity();
	return mesh;
}

CpuMesh convertLinesToMesh(const Vector<Line3D>& lines) {
	CpuMesh mesh;
	mesh.name = "Lines";
	mesh.dataFlags = Positions;
	mesh.drawMode = MeshDrawMode::DynamicMode;
	mesh.primitiveMode = PrimitiveMode::Lines;
	for (ULL i = 0; i < lines.size(); i++) {
		mesh.indices.push_back(i * 2);
		mesh.indices.push_back(i * 2 + 1);
		mesh.positions.push_back(lines[i].start);
		mesh.positions.push_back(lines[i].end);
	}
	mesh.checkIntegrity();
	return mesh;
}

void LineRenderer::renderLine(Renderer& renderer, const Vec2 start, const Vec2 end, const Float width) {
	renderLine(renderer, Vec3(start, 0), Vec3(end, 0), width);
}
