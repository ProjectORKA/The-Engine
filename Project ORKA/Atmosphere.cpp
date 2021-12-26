
#include "Atmosphere.hpp"
#include "Player.hpp"
#include "Renderer.hpp"

void Atmosphere::render(Player & player, Renderer& renderer) {
	Bool culling = renderer.getCulling();
	player.camera.renderOnlyRot(renderer);
	renderer.uniforms().data.cameraPosition = Vec4(player.camera.location,1);
	renderer.setCulling(false);
	renderer.uniforms().data.sunDir = Vec4(normalize(Vec3(0,1,1+sin(renderer.renderTime.total / 2))),1);
	renderer.uniforms().update();
	renderer.useShader("atmosphere");
	renderer.renderMesh("centeredCube");
	renderer.setCulling(culling);
}