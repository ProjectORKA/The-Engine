#include "DelaunayTriangulation.hpp"
#include "ProceduralMeshes.hpp"
#include "SimpleRTSTerrainSystem.hpp"
#include "Random.hpp"
#include "SimpleRts.hpp"
#include "SimplexNoise.hpp"

namespace
{
	Float smoothstep(const Float edge0, const Float edge1, const Float x)
	{
		const Float t = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	Float terrainColorNoise(const Double nx, const Double ny)
	{
		return static_cast<Float>(simplexOctaveNoise(
			sin(nx * glm::pi<Double>()),
			sin(ny * glm::pi<Double>()),
			cos(nx * glm::pi<Double>()),
			cos(ny * glm::pi<Double>()),
			3
		));
	}

	Vec3 proceduralTerrainColor(const SimpleRtsSimulation& sim, const Float height, const Double nx, const Double ny)
	{
		const SimpleRtsWorldSettings& settings = sim.worldSettings;

		if (height < SimpleRtsTerrainSystem::seaLevel)
		{
			const Float maxDepth = static_cast<Float>(settings.mapSize) / 64.0f;
			const Float depthT   = glm::clamp(-height / std::max(maxDepth, 1.0f), 0.0f, 1.0f);
			const Vec3 beachSand(0.62f, 0.56f, 0.42f);
			const Vec3 seabed(0.24f, 0.30f, 0.36f);
			return glm::mix(beachSand, seabed, depthT);
		}

		const Float maxLandHeight = static_cast<Float>(settings.mapSize) / 64.0f;
		const Float elevationT    = glm::clamp(height / std::max(maxLandHeight, 1.0f), 0.0f, 1.0f);
		const Float patch         = terrainColorNoise(nx * 4.0, ny * 4.0) * 0.5f + 0.5f;

		const Vec3 lowGrass(0.20f, 0.46f, 0.17f);
		const Vec3 midGrass(0.28f, 0.54f, 0.22f);
		const Vec3 rock(0.48f, 0.44f, 0.38f);

		Vec3 color = glm::mix(lowGrass, midGrass, smoothstep(0.0f, 0.35f, elevationT));
		color      = glm::mix(color, rock, smoothstep(0.55f, 1.0f, elevationT));
		color *= 0.88f + 0.12f * patch;
		return glm::clamp(color, Vec3(0.0f), Vec3(1.0f));
	}

	Vec3 proceduralOceanColor(const Double nx, const Double ny)
	{
		const Float patch = terrainColorNoise(nx * 2.5, ny * 2.5) * 0.5f + 0.5f;
		const Vec3 shallow(0.14f, 0.48f, 0.58f);
		const Vec3 open(0.05f, 0.22f, 0.42f);
		return glm::mix(shallow, open, patch * 0.65f);
	}

	void buildOceanMesh(CpuMesh& mesh, const SimpleRtsSimulation& sim)
	{
		mesh = CpuMesh{};
		if (!sim.worldSettings.enableWater) return;

		const Float extent = static_cast<Float>(sim.worldSettings.mapSize);
		proceduralPlaneMesh(mesh, extent, extent);
		for (Vec3& position : mesh.positions) position.z = SimpleRtsTerrainSystem::seaLevel;

		mesh.vertexColors.clear();
		mesh.vertexColors.reserve(mesh.positions.size());
		for (const Vec3& position : mesh.positions)
		{
			const Double nx = position.x / sim.worldSettings.mapSize;
			const Double ny = position.y / sim.worldSettings.mapSize;
			mesh.vertexColors.emplace_back(proceduralOceanColor(nx, ny));
		}

		mesh.name          = "simpleRtsOcean";
		mesh.drawMode      = BufferUsage::StaticDraw;
		mesh.primitiveMode = PrimitiveMode::Triangles;
		mesh.checkIntegrity();
	}
}

Double SimpleRtsTerrainSystem::sampleHeight(const SimpleRtsSimulation& sim, const Double a, const Double b) const
{
	const Double mapSize = static_cast<Double>(sim.worldSettings.mapSize);
	const SimpleRtsWorldSettings& settings = sim.worldSettings;

	if (!settings.enableElevation)
		return settings.enableWater ? 1.0 : 0.0;

	Double height1 = simplexOctaveNoise(
		sin(a * glm::pi<Double>()),
		sin(b * glm::pi<Double>()),
		cos(a * glm::pi<Double>()),
		cos(b * glm::pi<Double>()),
		8
	);

	height1 *= 2;
	height1 *= abs(height1);

	if (height1 < 0.0 && !settings.enableWater) return 1.0;

	return height1 * mapSize / 64.0;
}

Bool SimpleRtsTerrainSystem::isWater(const SimpleRtsSimulation& sim, const Vec2 position) const
{
	if (!sim.worldSettings.enableWater) return false;
	return sampleHeight(sim, position.x / sim.worldSettings.mapSize, position.y / sim.worldSettings.mapSize) <= 0.0;
}

void SimpleRtsTerrainSystem::create(const SimpleRtsSimulation& sim)
{
	name = "TerrainSystem";
	terrainMesh = CpuMesh{};
	const Int pointCount = sim.worldSettings.terrainPointCount;
	Vector<Double> points;
	points.reserve(static_cast<SizeT>(pointCount) * 2);

	for (Int i = 0; i < pointCount; i++)
	{
		Double a = randomDoubleFast(-sim.worldSettings.mapSize, sim.worldSettings.mapSize);
		Double b = randomDoubleFast(-sim.worldSettings.mapSize, sim.worldSettings.mapSize);
		points.emplace_back(a);
		points.emplace_back(b);
		const Double nx      = a / sim.worldSettings.mapSize;
		const Double ny      = b / sim.worldSettings.mapSize;
		const Float height   = static_cast<Float>(sampleHeight(sim, nx, ny));
		terrainMesh.positions.emplace_back(a, b, height);
		terrainMesh.vertexColors.emplace_back(proceduralTerrainColor(sim, height, nx, ny));
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

	buildOceanMesh(oceanMesh, sim);
}

void SimpleRtsTerrainSystem::destroy(const SimpleRtsSimulation& sim)
{
	terrainMesh = CpuMesh{};
	oceanMesh   = CpuMesh{};
}
