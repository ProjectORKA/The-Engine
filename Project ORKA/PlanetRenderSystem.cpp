
#include "PlanetRenderSystem.hpp"
#include "Renderer.hpp"

void PlanetRenderSystem::destroy()
{
	octreeRenderSystem.destroy();
	quadtreeRenderSystem.destroy();
}
void PlanetRenderSystem::create(Renderer & renderer)
{
	quadtreeRenderSystem.create(renderer);
	octreeRenderSystem.create(renderer);
}
void PlanetRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera & planetCamera)
{
	//create if necessary
	if (octreeRenderSystem.root.equivalentOctreeNode == nullptr) octreeRenderSystem.root.create(*planetSystem.octreeSystem.root);
	//if (quadtreeRenderSystem.root.equivalentQuadtreeNode == nullptr) quadtreeRenderSystem.root.create(*planetSystem.quadtreeSystem.root);
	
	//update before rendering
	octreeRenderSystem.update(planetCamera);
	//quadtreeRenderSystem.update(planetCamera);

	//set uniforms
	planetCamera.use(renderer.uniforms(), renderer.renderRegion.getAspectRatio());
	renderer.uniforms().data.distortion = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.uniforms().data.mMatrix = Matrix(1);

	renderer.setDepthTest(true);
	renderer.setCulling(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {

		renderer.clearDepth();

		octreeRenderSystem.renderLevel(level, renderer);

		//render terrain
		//renderer.shaderSystem.use(quadtreeRenderSystem.terrainShaderID);
		//renderer.textureSystem.use(quadtreeRenderSystem.grassTextureID);
		//quadtreeRenderSystem.renderLevel(level, renderer);
	}
}