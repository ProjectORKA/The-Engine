
//#include "Atmosphere.hpp"
//#include "Player.hpp"
//#include "Renderer.hpp"
//
//void Atmosphere::render(Sun & sun, Player & player, Renderer& renderer) {
//	Bool culling = renderer.getCulling();
//	player.camera.renderOnlyRot(renderer);
//	renderer.uniforms().cameraPos() = Vec4(player.camera.location, 1);
//	renderer.setCulling(false);
//	renderer.uniforms().sunDir() = Vec4(sun.sunDirection, 1);
//	renderer.useShader("atmosphere");
//	renderer.renderMesh("centeredCube");
//	renderer.setCulling(culling);
//}