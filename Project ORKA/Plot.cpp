#include "Plot.hpp"
#include"Renderer.hpp"

void Plot::add(const Vec2 value)
{
	points.push_back(value);
}

CPUMesh Plot::createMesh() const
{
	CPUMesh mesh;

	mesh.drawMode      = BufferUsage::DynamicDraw;
	mesh.name          = "plot";
	mesh.primitiveMode = PrimitiveMode::LineStrip;

	for(UInt index = 0; index < points.size(); index++)
	{
		Vec2 pointPos = points[index];

		mesh.positions.emplace_back(pointPos, 0);
		mesh.textureCoordinates.push_back(pointPos);
		mesh.normals.emplace_back(0, 0, 1);

		mesh.indices.push_back(index);
	}

	mesh.checkIntegrity();
	return mesh;
}

void Plot::render(Renderer& renderer) const
{
	renderer.line(points);
}

void Plot::add(const Float a, const Float b)
{
	add(Vec2(a, b));
}
