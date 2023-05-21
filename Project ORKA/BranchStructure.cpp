#include "BranchStructure.hpp"

#include "Renderer.hpp"

void BranchStructure::render(Renderer& renderer, const Vector<Vec3>& points) const {
	CpuMesh mesh;
	mesh.drawMode = MeshDrawMode::DynamicMode;
	mesh.name = "branchStructure";
	mesh.normals = Vector<Vec3>(points.size(), Vec3(0, 0, 1));
	mesh.primitiveMode = PrimitiveMode::Lines;
	mesh.positions = points;

	for (int i = 0; i < connections.size(); i++) {
		mesh.indices.push_back(i);
		mesh.indices.push_back(connections[i]);
	}

	for (auto point : points) { mesh.textureCoordinates.push_back(Vec2(point.x, point.y)); }

	mesh.checkIntegrity();

	mesh.render(renderer);
}
