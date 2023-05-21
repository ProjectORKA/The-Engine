#include "PointCloud.hpp"
#include "Renderer.hpp"
#include "CpuMesh.hpp"

void PointCloud::add(const Vec3 point) { points.push_back(point); }

void PointCloudRenderer::update(PointCloud& pointCloud) {
	if (gpuMesh.loaded)gpuMesh.unload();
	CpuMesh mesh;
	mesh.drawMode = MeshDrawMode::DynamicMode;
	mesh.name = "PointCloud";
	mesh.primitiveMode = PrimitiveMode::Points;

	Int i = 0;
	for (Vec3& point : pointCloud.points) {
		mesh.indices.push_back(i);
		mesh.normals.push_back(Vec3(0, 0, 1));
		mesh.textureCoordinates.push_back(Vec2(point.x, point.y));
		mesh.positions.push_back(point);
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
