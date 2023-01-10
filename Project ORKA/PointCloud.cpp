
#include "PointCloud.hpp"
#include "Renderer.hpp"
#include "CPUMesh.hpp"

void PointCloud::add(Vec3 point) {
	points.pushBack(point);
}

void PointCloudRenderer::update(PointCloud& pointCloud) {
	if (gpuMesh.loaded)gpuMesh.unload();
	CPUMesh mesh;
	mesh.drawMode = MeshDrawMode::dynamicMode;
	mesh.name = "PointCloud";
	mesh.primitiveMode = PrimitiveMode::Points;

	Int i = 0;
	for (Vec3& point : pointCloud.points) {
		mesh.indices.pushBack(i);
		mesh.normals.pushBack(Vec3(0, 0, 1));
		mesh.uvs.pushBack(Vec2(point.x, point.y));
		mesh.vertices.pushBack(point);
		i++;
	}
	mesh.checkIntegrity();

	gpuMesh.upload(mesh);

	pointCloudSize = pointCloud.points.size();
}
void PointCloudRenderer::render(PointCloud& pointCloud, Renderer& renderer) {
	if (pointCloud.points.size() != pointCloudSize) update(pointCloud);
	gpuMesh.render(renderer.uniforms());
}