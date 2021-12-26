
#pragma once

#include "Basics.hpp"
#include "Renderer.hpp"


struct PointCloud {
	Vector<Vec3> points;

	void add(Vec3 point) {
		points.push_back(point);
	};
	void render() {
		GPUMesh mesh;
		mesh.upload(createMesh());
		mesh.render();
		mesh.unload();
	}

	CPUMesh createMesh() {
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
	};
};