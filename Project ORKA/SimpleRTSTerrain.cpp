
#include "SimpleRTSTerrain.hpp"

void SimpleRTSTerrainRenderingSystem::create(ResourceManager& resourceManager) {
	cpuTerrain.load(resourceManager, "simplertsTerrain");
	gpuTerrain.upload(cpuTerrain);
}
void SimpleRTSTerrainRenderingSystem::render(Renderer& renderer) {
	renderer.uniforms().mMatrix(matrixFromLocation(offset));
	gpuTerrain.render(renderer.uniforms());
}
Float SimpleRTSTerrainSystem::simpleRTSTerrainFunction(Vec2 position) {
	Float height = 15 * noise.octaveNoise0_1(position.x, position.y, 16);
	//logDebug(height);
	return height;
}
void SimpleRTSTerrainRenderingSystem::update(SimpleRTSTerrainSystem & terrainSystem, Vec2 cameraPos) {
	offset = cameraPos;

	Float size = 100;

	for (Vec3& v : cpuTerrain.positions) {
		v.z = terrainSystem.simpleRTSTerrainFunction((offset + Vec2(v.x, v.y)) / size);
	}

	cpuTerrain.calculateSmoothNormals();

	gpuTerrain.unload();
	gpuTerrain.upload(cpuTerrain);
}