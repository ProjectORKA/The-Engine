
#include "Plot.hpp"

void Plot::render() {
	GPUMesh mesh;
	mesh.upload(createMesh());
	mesh.render();
}
void Plot::add(Vec2 value) {
	points.push_back(value);
}
CPUMesh Plot::createMesh() {
	CPUMesh mesh;

	mesh.drawMode = MeshDrawMode::dynamicMode;
	mesh.name = "plot";
	mesh.primitiveMode = PrimitiveMode::LineStrip;

	for (UInt index = 0; index < points.size(); index++) {
		Vec2 pointPos = points[index];

		mesh.vertices.push_back(Vec3(pointPos, 0));
		mesh.uvs.push_back(pointPos);
		mesh.normals.push_back(Vec3(0, 0, 1));

		mesh.indices.push_back(index);
	}

	mesh.checkIntegrity();
	return mesh;
}
void Plot::add(Float a, Float b) {
	add(Vec2(a, b));
}