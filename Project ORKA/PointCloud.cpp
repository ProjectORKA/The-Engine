
#include "PointCloud.hpp"
#include "Renderer.hpp"
#include "CPUMesh.hpp"

void PointCloud::add(Vec3 point) {
	points.push_back(point);
}

void PointCloud::render(Renderer& renderer) {
	GPUMesh mesh;
	mesh.upload(createMesh());
	mesh.render(renderer.uniforms());
	mesh.unload();
}

CPUMesh PointCloud::createMesh() {
	CPUMesh mesh;
	mesh.drawMode = MeshDrawMode::dynamicMode;
	mesh.name = "PointCloud";
	mesh.primitiveMode = PrimitiveMode::Points;

	Int i = 0;
	for (Vec3& point : points) {
		mesh.indices.push_back(i);
		mesh.normals.push_back(Vec3(0, 0, 1));
		mesh.uvs.push_back(Vec2(point.x, point.y));
		mesh.vertices.push_back(point);
		i++;
	}
	mesh.checkIntegrity();
	return mesh;
}
