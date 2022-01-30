
#include "PlanetRenderSystem.hpp"
#include "Renderer.hpp"
#include "PlanetSystem.hpp"
#include "PlanetSystemPlayer.hpp"

void PlanetRenderSystem::destroy()
{
	octreeRenderSystem.destroy();
	quadtreeRenderSystem.destroy();
}
void PlanetRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer, PlanetSystemPlayer& player)
{
	//create if necessary
	//if (octreeRenderSystem.root.equivalentOctreeNode == nullptr) octreeRenderSystem.root.create(*planetSystem.octreeSystem.root);
	if (quadtreeRenderSystem.root.equivalentQuadtreeNode == nullptr) quadtreeRenderSystem.root.create(*planetSystem.quadtreeSystem.root);
	
	//update before rendering
	//octreeRenderSystem.update(planetCamera);
	quadtreeRenderSystem.update(player);

	//set uniforms
	renderer.uniforms().customInt1() = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().mMatrix() = Matrix(1);
	player.camera.renderOnlyRot(renderer);

	renderer.setDepthTest(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {

		renderer.clearDepth();

		//octreeRenderSystem.renderLevel(level, renderer);

		//render terrain
		renderer.shaderSystem.use("terrain");
		renderer.textureSystem.use("terrainColor");
		quadtreeRenderSystem.renderLevel(level, renderer);
	}
}