#include "SimpleRTSTerrain.hpp"
#include "PerlinNoise.hpp"

void SimpleRTSTerrainRenderingSystem::render(Renderer& renderer) const
{
	renderer.uniforms().setMMatrix(matrixFromLocation(offset));
	gpuTerrain.render(renderer.uniforms());
}

void SimpleRTSTerrainRenderingSystem::create(ResourceManager& resourceManager)
{
	cpuTerrain.load(resourceManager, "simplertsTerrain");
	gpuTerrain.upload(cpuTerrain);
}

Float SimpleRTSTerrainSystem::simpleRTSTerrainFunction(const Vec2 position) const
{
	const Float height = 15.0f * static_cast<Float>(noise.octaveNoise0_1(position.x, position.y, 16.0f));
	return height;
}

void SimpleRTSTerrainRenderingSystem::update(const SimpleRTSTerrainSystem& terrainSystem, const Vec2 cameraPos)
{
	offset = cameraPos;

	constexpr Float size = 100;

	for(Vec3& v : cpuTerrain.positions) v.z = terrainSystem.simpleRTSTerrainFunction((offset + Vec2(v.x, v.y)) / size);

	cpuTerrain.calculateSmoothNormals();

	gpuTerrain.unload();
	gpuTerrain.upload(cpuTerrain);
}
