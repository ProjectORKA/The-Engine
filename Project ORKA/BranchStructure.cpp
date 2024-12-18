#include "BranchStructure.hpp"
#include "Renderer.hpp"

void BranchStructure::render(Renderer& renderer, const Vec3Vector& points) const
{
	CpuMesh mesh;
	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.name          = "branchStructure";
	mesh.normals       = Vec3Vector(points.size(), Vec3(0, 0, 1));
	mesh.primitiveMode = PrimitiveMode::Lines;
	mesh.positions     = points;

	for(SizeT i = 0; i < connections.size(); i++)
	{
		mesh.indices.push_back(i);
		mesh.indices.push_back(connections[i]);
	}

	for(auto& point : points) mesh.textureCoordinates.emplace_back(point.x, point.y);

	mesh.checkIntegrity();

	mesh.render(renderer);
}
