#include "PlanetSystemPlayer.hpp"
#include "Window.hpp"

void PlanetSystemPlayer::render(ResourceManager& resourceManager, Window& window) {
	const Float cameraHeight = chunkLocation.z + camera.location.z;
	window.renderer.uniforms().custom1() = cameraHeight;
	window.renderer.uniforms().cameraVec() = Vec4(camera.forwardVector, 1);
	window.renderer.uniforms().cameraPos() = Vec4(0);
	window.renderer.uniforms().vMatrix() = camera.viewMatrixOnlyRot();
	window.renderer.uniforms().pMatrix() = camera.projectionMatrix(window.renderer.getAspectRatio());
}
