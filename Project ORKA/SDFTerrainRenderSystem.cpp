
#include "SDFTerrainRenderSystem.hpp"
#include "Renderer.hpp"

void SDFTerrainRenderSystem::render(Renderer& renderer) {

	renderer.setCulling(false);
	renderer.setDepthTest(false);

	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.useShader("sdfTerrain");
	renderer.useTexture("altitudeColor");
	renderer.useTexture("terrain",1);
	renderer.useTexture("terrainColor", 2);
	renderer.renderMesh("sky");
}
