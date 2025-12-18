#include "DelaunayTriangulation.hpp"
#include "SimpleRTSTerrainSystem.hpp"
#include "PerlinNoise.hpp"
#include "Random.hpp"
#include "SimpleRts.hpp"
#include "SimplexNoise.hpp"

Double SimpleRtsTerrainSystem::terrainFunction(const Double a, const Double b, const Double mapSize) const
{
	Double height1 = simplexOctaveNoise(
		sin(a * glm::pi<Double>()),
		sin(b * glm::pi<Double>()),
		cos(a * glm::pi<Double>()),
		cos(b * glm::pi<Double>()),
		8
	);

	height1 *= 2;
	height1 *= abs(height1);

	Double height2 = simplexOctaveNoise(
		sin((2*a+100) * glm::pi<Double>()),
		sin((2*b+100) * glm::pi<Double>()),
		cos((2*a+100) * glm::pi<Double>()),
		cos((2*b+100) * glm::pi<Double>()),
		8
	);

	height2 *= abs(height2);
	height2 *= abs(height2);
	height2 *= 2;

	const Double height = height1; //max(height1, height2);

	if (height < 0.0) return 0;
	return height * mapSize/64.0;
}

Bool SimpleRtsTerrainSystem::isWater(const SimpleRtsSimulation& sim, const Vec2 position) const
{
	return terrainFunction(position.x/sim.mapSize, position.y/sim.mapSize, sim.mapSize) <= 0.0;
}

void SimpleRtsTerrainSystem::create(const SimpleRtsSimulation& sim)
{
	name = "TerrainSystem";
	Vector<Double> points;
	points.reserve(count * 2);

	for (Int i = 0; i < count; i++)
	{
		Double a = randomDoubleFast(-sim.mapSize, sim.mapSize);
		Double b = randomDoubleFast(-sim.mapSize, sim.mapSize);
		points.emplace_back(a);
		points.emplace_back(b);
		const Float height =  terrainFunction(a/sim.mapSize, b/sim.mapSize, sim.mapSize);
		terrainMesh.positions.emplace_back(a, b, height);

		if (height <= 0.0) terrainMesh.textureCoordinates.emplace_back(0.260646, 0.811398);
		else terrainMesh.textureCoordinates.emplace_back(0.020, 0.674); 
		
		terrainMesh.vertexColors.emplace_back(0, 0, 0);
	}
	terrainMesh.name          = "terrain";
	terrainMesh.drawMode      = BufferUsage::StaticDraw;
	terrainMesh.primitiveMode = PrimitiveMode::Triangles;

	const Delaunator::Delaunator d(points);

	for (SizeT i = 0; i < d.triangles.size(); i += 3)
	{
		terrainMesh.indices.emplace_back(d.triangles[i + 2]);
		terrainMesh.indices.emplace_back(d.triangles[i + 1]);
		terrainMesh.indices.emplace_back(d.triangles[i]);
	}
	terrainMesh.calculateFlatNormals();
	terrainMesh.checkIntegrity();
}