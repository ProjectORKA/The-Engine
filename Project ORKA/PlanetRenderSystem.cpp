
#include "PlanetRenderSystem.hpp"
#include "Renderer.hpp"

void PlanetRenderSystem::destroy()
{
	if(octreeNodeRenderData.parentNode)octreeNodeRenderData.destroy();
	if (quadtreeNodeRenderData.parentNode)quadtreeNodeRenderData.destroy();
}
void PlanetRenderSystem::render(PlanetSystem& planetSystem, Renderer& renderer)
{
	//dynamicallyUpdateDrawDistance(renderer.targetFrameRate, renderer.renderTime);
	
	renderer.cameraSystem.currentPlanetCamera().render(renderer.uniforms(), renderer.currentViewport());
	renderer.shaderSystem.use("terrain");
	renderer.textureSystem.use("noise");
	renderer.shaderSystem.uniforms.setBool("distortion", worldDistortion);
	renderer.uniforms().setMatrix("mMatrix", Matrix(1));
	renderer.setDepthTest(true);
	renderer.setCulling(true);
	
	//octree stuff
	//if (!octreeNodeRenderData.parentNode) octreeNodeRenderData.create(planetSystem.octreeRoot);
	//octreeNodeRenderData.update(renderer.cameraSystem.currentPlanetCamera(),drawDistance);
	//
	//for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {
	//	glClear(GL_DEPTH_BUFFER_BIT);
	//	octreeNodeRenderData.renderLevel(level, chunkBorders, renderer.meshSystem, renderer.textureSystem, renderer.shaderSystem);
	//}

	//quadtree stuff
	if (!quadtreeNodeRenderData.parentNode) quadtreeNodeRenderData.create(planetSystem.quadtreeRoot);
	quadtreeNodeRenderData.update(renderer.cameraSystem.currentPlanetCamera());
	
	for (UShort level = 0; level < MAX_CHUNK_LEVEL; level++) {
		glClear(GL_DEPTH_BUFFER_BIT);
		quadtreeNodeRenderData.renderLevel(level, renderer.shaderSystem);
	}

}
void PlanetRenderSystem::dynamicallyUpdateDrawDistance(Float targetFrameRate, Time& rendererTime) {
	//average fps, to avoid loopback
	static float averageDelta = 0.16;

	bool smoothing = false;

	if (smoothing) {
		float oldToNewRatio = 2 * rendererTime.delta / averageDelta; // >1 means framerate dropped
		//the code below gradually introduces the new values
		averageDelta += oldToNewRatio * rendererTime.delta;
		averageDelta /= (1 + oldToNewRatio);
	}
	else {
		averageDelta = rendererTime.delta;
	}

	//get current fps relative to target
	//>1 means too low fps <1 means too high
	static float timePerFrameRatio = 1;
	timePerFrameRatio = averageDelta * targetFrameRate;


		float lerpFactor = rendererTime.delta/2;
		drawDistance += lerpFactor * (drawDistance / timePerFrameRatio);
		drawDistance /= 1 + lerpFactor;

	drawDistance = clamp(drawDistance, 1.4f, 10.0f);
}