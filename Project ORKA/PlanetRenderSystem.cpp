
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
void PlanetRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player)
{
	//set uniforms
	renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);

	renderer.setDepthTest(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {

		renderer.clearDepth();

		//render terrain
		//renderer.shaderSystem.use("terrain");
		renderer.shaderSystem.use("MooncrashVertex");
		renderer.textureSystem.use("terrainColor");
		quadtreeRenderSystem.renderLevel(level, renderer);
	}
}
void PlanetRenderSystem::renderLevel(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level) {

	//set uniforms
	renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);

	renderer.setDepthTest(true);

	renderer.clearDepth();

	//set shading
	renderer.shaderSystem.use("terrain");
	renderer.textureSystem.use("terrainColor");

	quadtreeRenderSystem.renderLevel(level, renderer);

	//set shading
	renderer.shaderSystem.use("debug");
	quadtreeRenderSystem.debugRenderLevel(level, renderer);
}