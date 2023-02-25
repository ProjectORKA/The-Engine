#include "LineRenderer.hpp"
#include "Renderer.hpp"

void LineRenderer::create() {
	cpuMesh.drawMode = MeshDrawMode::dynamicMode;
	cpuMesh.indices.pushBack(0);
	cpuMesh.indices.pushBack(1);
	cpuMesh.indices.pushBack(2);
	cpuMesh.indices.pushBack(3);
	cpuMesh.name = "line";
	cpuMesh.normals.pushBack(Vec3(0, 0, 1));
	cpuMesh.normals.pushBack(Vec3(0, 0, 1));
	cpuMesh.normals.pushBack(Vec3(0, 0, 1));
	cpuMesh.normals.pushBack(Vec3(0, 0, 1));
	cpuMesh.primitiveMode = PrimitiveMode::TriangleStrip;
	cpuMesh.textureCoordinates.pushBack(Vec2(0, 1));
	cpuMesh.textureCoordinates.pushBack(Vec2(0, 0));
	cpuMesh.textureCoordinates.pushBack(Vec2(1, 0));
	cpuMesh.textureCoordinates.pushBack(Vec2(1, 1));
	cpuMesh.vertexColors.pushBack(Vec3(1));
	cpuMesh.vertexColors.pushBack(Vec3(1));
	cpuMesh.vertexColors.pushBack(Vec3(1));
	cpuMesh.vertexColors.pushBack(Vec3(1));
	cpuMesh.positions.resize(4);
}
void LineRenderer::renderLine(Renderer& renderer, Vec3 start, Vec3 end, Float width) {
	renderer.uniforms().mMatrix(Matrix(1));
	Vec3 dir = normalize(start - end);
	Vec3 extend = cross(dir, Vec3(0, 0, 1));
	cpuMesh.positions[0] = (start + extend * width);
	cpuMesh.positions[1] = (start - extend * width);
	cpuMesh.positions[2] = (end + extend * width);
	cpuMesh.positions[3] = (end - extend * width);

	cpuMesh.checkIntegrity();
	gpuMesh.upload(cpuMesh);
	gpuMesh.render(renderer.uniforms());
	gpuMesh.unload();
}
void LineRenderer::renderLine(Renderer& renderer, Vec2 start, Vec2 end, Float width) {
	renderLine(renderer,Vec3(start, 0), Vec3(end, 0), width);
}