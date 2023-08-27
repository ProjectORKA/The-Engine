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

		mesh.positions.push_back(Vec3(pointPos, 0));
		mesh.textureCoordinates.push_back(pointPos);
		mesh.normals.push_back(Vec3(0, 0, 1));

		mesh.indices.push_back(index);
	}

	mesh.checkIntegrity();
	return mesh;
}

void Plot::render(Renderer& renderer) const
{
	Vec2 last = points[0];

	for(Vec2 p : points)
	{
		renderer.lineRenderer.renderLine(renderer, Vec3(last, 0), Vec3(p, 0));
		last = p;
	}
}

void Plot::add(const Float a, const Float b)
{
	add(Vec2(a, b));
}
