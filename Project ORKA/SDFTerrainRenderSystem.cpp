
#include "SDFTerrainRenderSystem.hpp"
#include "Renderer.hpp"

void SDFTerrainRenderSystem::render(Renderer& renderer) {

	renderer.setCulling(false);
	renderer.setDepthTest(false);

	renderer.shaderSystem.uniforms.data.mMatrix = Matrix(1);
	renderer.shaderSystem.use("sdfTerrain");
	renderer.textureSystem.use("altitudeColor");
	renderer.textureSystem.use("terrain",1);
	renderer.textureSystem.use("terrainColor", 2);
	renderer.meshSystem.render("sky");
}
