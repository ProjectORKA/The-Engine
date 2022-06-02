
#include "SDFTerrainRenderSystem.hpp"
#include "Renderer.hpp"

void SDFTerrainRenderSystem::render(Renderer& renderer) {
	renderer.setCulling(false);
	renderer.setDepthTest(false);
	renderer.uniforms().mMatrix(Matrix(1));
	renderer.useShader("sdfTerrain");
	renderer.framebufferSystem.currentDraw().setAsTexture(0); //[TODO]might not work if draw doesent bind; check
	renderer.useTexture("noise",1);
	renderer.renderMesh("sky");
}
