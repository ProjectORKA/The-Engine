#include "PlanetSystemPlayer.hpp"
#include "Window.hpp"

void PlanetSystemPlayer::render(Window& window)
{
	const Float cameraHeight = chunkLocation.z + camera.getLocation().z;
	window.renderer.uniforms().setCustom1(cameraHeight);
	window.renderer.uniforms().setCameraVec(Vec4(camera.getForwardVector(), 1));
	window.renderer.uniforms().setCameraPos(Vec4(0));
	window.renderer.uniforms().setVMatrix(camera.viewMatrixOnlyRot());
	window.renderer.uniforms().setPMatrix(camera.projectionMatrix(window.renderer.getAspectRatio()));
}
