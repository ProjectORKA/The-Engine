
#include "Plot.hpp"
#include"Renderer.hpp"

void Plot::add(Vec2 value) {
	points.pushBack(value);
}
CPUMesh Plot::createMesh() {
	CPUMesh mesh;

	mesh.drawMode = MeshDrawMode::dynamicMode;
	mesh.name = "plot";
	mesh.primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < points.size(); index++) {
		Vec2 pointPos = points[index];

		mesh.vertices.pushBack(Vec3(pointPos, 0));
		mesh.uvs.pushBack(pointPos);
		mesh.normals.pushBack(Vec3(0, 0, 1));

		mesh.indices.pushBack(index);
	}

	mesh.checkIntegrity();
	return mesh;
}
void Plot::add(Float a, Float b) {
	add(Vec2(a, b));
}
void Plot::render(Renderer& renderer) {
	Vec2 last = points[0];

	for (Vec2 p : points) {
		renderer.lineRenderer.renderLine(renderer, Vec3(last, 0), Vec3(p, 0), 1);
		last = p;
	}
}
void Plot::render(Renderer & renderer, Float linewidth) {
	Vec2 last = points[0];

	for (Vec2 p : points) {
		renderer.lineRenderer.renderLine(renderer, Vec3(last, 0), Vec3(p, 0), linewidth);
		last = p;
	}
}