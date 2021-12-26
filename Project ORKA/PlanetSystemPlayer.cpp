#include "PlanetSystemPlayer.hpp"
#include "Renderer.hpp"

void PlanetSystemPlayer::render(Renderer& renderer) {
	Float cameraHeight = chunkLocation.z + camera.location.z;
	renderer.uniforms().data.custom1 = cameraHeight;
	renderer.uniforms().data.cameraVector = Vec4(camera.forwardVector, 1);
	renderer.uniforms().data.cameraPosition = Vec4(0);
	renderer.uniforms().data.vMatrix = camera.viewMatrixOnlyRot();
	renderer.uniforms().data.pMatrix = camera.projectionMatrix(renderer.aspectRatio());
	renderer.uniforms().update();
}
