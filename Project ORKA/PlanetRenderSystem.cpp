
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
	if (quadtreeRenderSystem.root.equivalentQuadtreeNode == nullptr) quadtreeRenderSystem.root.create(planetSystem.quadtreeSystem.root);
	
	octreeRenderSystem.update(planetCamera);
	quadtreeRenderSystem.update(planetCamera);

	planetCamera.render(renderer.uniforms(), renderer.renderRegion.getAspectRatio());

	renderer.shaderSystem.uniforms.data.distortion = Int(renderer.planetRenderSystem.worldDistortion);
	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);

	renderer.setDepthTest(true);
	renderer.setCulling(true);

	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {
		
		renderer.setDepthTest(false);
		renderer.setCulling(false);

		renderer.clearDepth();

		//renderer.shaderSystem.use("depthCutter");
		//renderer.framebufferSystem.current().depthTexture.use(0);
		//renderer.shaderSystem.uniforms.update();
		//renderer.meshSystem.render("fullScreenQuad");

		renderer.setDepthTest(true);
		renderer.setCulling(true);

		//render terrain
		renderer.shaderSystem.use(quadtreeRenderSystem.terrainShaderID);
		renderer.textureSystem.use(quadtreeRenderSystem.grassTextureID);
		quadtreeRenderSystem.renderLevel(level, renderer);
		
		//renderer.shaderSystem.use("primitive");
		//renderer.textureSystem.use("default");
		//octreeRenderSystem.renderLevel(level, renderer);

		//render water
		//renderer.setDepthTest(false);
		//glDepthMask(GL_FALSE);
		//renderer.setCulling(false);
		//renderer.shaderSystem.use("water");
		//
		//renderer.uniforms().data.chunkOffsetVector = Vec4((Vec3(planetCamera.chunkLocation) + planetCamera.location) / Vec3(ULLONG_MAX) ,1);
		//renderer.uniforms().data.mMatrix = Matrix(1);
		//renderer.framebufferSystem.current().depthTexture.use(0);
		//renderer.uniforms().update();
		//renderer.meshSystem.render("centeredCube");

		//glDepthMask(GL_TRUE);
	}
}