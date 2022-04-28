
#include "SDFTerrainRenderSystem.hpp"
#include "Renderer.hpp"

void SDFTerrainRenderSystem::render(Renderer& renderer) {
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.uniforms().mMatrix(Matrix(1));
	renderer.useShader("sdfTerrain");
	renderer.framebufferSystem.current().setAsTexture(0);
	renderer.useTexture("noise",1);
	renderer.renderMesh("sky");
}
