
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
void PlanetRenderSystem::renderAllLevels(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player)
{
	//renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);
	renderer.setDepthTest(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {
		renderer.clearDepth();
		if(vertexColors)renderer.shaderSystem.use("mooncrashVertexColor");
		else renderer.shaderSystem.use("terrain");
		renderer.textureSystem.use("terrainColor");
		quadtreeRenderSystem.renderLevel(level, renderer);
	}
}
void PlanetRenderSystem::renderLevel(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player, UShort level) {

	//renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);
	renderer.setDepthTest(true);

	renderer.clearDepth();
	if (vertexColors)renderer.shaderSystem.use("mooncrashVertexColor");
	else renderer.shaderSystem.use("terrain");
	renderer.textureSystem.use("terrainColor");
	quadtreeRenderSystem.renderLevel(level, renderer);
}