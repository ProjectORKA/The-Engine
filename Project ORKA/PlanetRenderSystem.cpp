
#include "PlanetRenderSystem.hpp"
#include "Renderer.hpp"
#include "PlanetSystem.hpp"
#include "PlanetSystemPlayer.hpp"

void PlanetRenderSystem::destroy()
{
	quadtreeRenderSystem.destroy();
}
void PlanetRenderSystem::update(PlanetSystem& planetSystem, PlanetSystemPlayer& player) {
	//create if necessary
	if (quadtreeRenderSystem.root.equivalentQuadtreeNode == nullptr) quadtreeRenderSystem.root.create(*planetSystem.quadtreeSystem.root);

	//update before rendering
	quadtreeRenderSystem.update(player);
}
void PlanetRenderSystem::renderLevel(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level) {
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);
	renderer.setDepthTest(true);

	renderer.clearDepth();
	if (vertexColors)renderer.shaderSystem.use("mooncrashVertexColor");
	else renderer.shaderSystem.use("terrain");
	renderer.textureSystem.use("terrainColor");
	quadtreeRenderSystem.renderTerrainTileLevel(level, renderer);
}

void ClipMap::update(PlanetSystem& planetSystem) {
	for (int x = 0; x < heightmap.size; x++) {
		for (int y = 0; y < heightmap.size; y++) {
			heightmap.set(x, y, randomFloat(0,0.1));
		}
	}

	mesh.unload();

	CPUMesh tmp;
	tmp.triangleMeshFromHeightmap(heightmap);
	tmp.calculateSmoothNormals();
	mesh.upload(tmp);
}

void ClipMap::render(Renderer& renderer) {
	mesh.render(renderer.uniforms());
}

void PlanetRenderSystem2::create(PlanetSystem& planetSystem) {
	clipMaps.resize(MAX_CHUNK_LEVEL);
}

void PlanetRenderSystem2::update(PlanetSystem& planetSystem) {
	if (clipMaps.size() < MAX_CHUNK_LEVEL)
	{

		clipMaps.resize(MAX_CHUNK_LEVEL);

		for (int i = 0; i < MAX_CHUNK_LEVEL; i++) {
			clipMaps[i].update(planetSystem);
		}
	}
}

void PlanetRenderSystem2::render(Renderer& renderer, PlanetSystemPlayer& player, UShort level) {
	Matrix m = Matrix(1);

	Vec3 pos = Vec3(player.chunkLocation) / Vec3(ULLONG_MAX/2);

	m = translate(m, -pos);

	renderer.uniforms().mMatrix(m);

	player.camera.renderOnlyRot(renderer); //can be optimized

	//renderer.setDepthTest(false);
	//renderer.setCulling(false);

	renderer.useShader("terrain");
	clipMaps[level].render(renderer);
}
