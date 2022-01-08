#include "PlanetSystemPlayer.hpp"
#include "Renderer.hpp"

void PlanetSystemPlayer::render(Renderer& renderer) {
	Float cameraHeight = chunkLocation.z + camera.location.z;
	renderer.uniforms().custom1() = cameraHeight;
	renderer.uniforms().cameraVec() = Vec4(camera.forwardVector, 1);
	renderer.uniforms().cameraPos() = Vec4(0);
	renderer.uniforms().vMatrix() = camera.viewMatrixOnlyRot();
	renderer.uniforms().pMatrix() = camera.projectionMatrix(renderer.aspectRatio());
}
