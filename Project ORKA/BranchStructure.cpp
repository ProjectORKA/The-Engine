
#include "BranchStructure.hpp"

#include "Renderer.hpp"

void BranchStructure::render(Renderer& renderer, Vector<Vec3> points) {
	CPUMesh mesh;
	mesh.drawMode = MeshDrawMode::dynamicMode;
	mesh.name = "branchStructure";
	mesh.normals = Vector<Vec3>(points.size(), Vec3(0, 0, 1));
	mesh.primitiveMode = PrimitiveMode::Lines;
	mesh.positions = points;

	for (int i = 0; i < connections.size(); i++) {
		mesh.indices.pushBack(i);
		mesh.indices.pushBack(connections[i]);
	}

	for (int i = 0; i < points.size(); i++) {
		mesh.textureCoordinates.pushBack(Vec2(points[i].x, points[i].y));
	}

	mesh.checkIntegrity();

	mesh.render(renderer);
}
