
#include "Miniverse.hpp"
#include "Window.hpp"

void Miniverse::render(TiledRectangle area, Renderer& renderer) {
	//update camera right before rendering
	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	//preprocess
	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.renderSky(player.camera);

	renderer.renderAtmosphere(player, normalize(Vec3(1)));
	renderer.clearDepth();
	//scene
	renderer.setAlphaBlending(false);

	player.render(renderer);

	renderer.useShader("miniverseShader");
	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh("monkey");
}
void Miniverse::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void Miniverse::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void Miniverse::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::LEFT && action == ActionState::Press) inputManager.captureCursor(window);
	if (button == MouseButton::RIGHT && action == ActionState::Press) inputManager.uncaptureCursor(window);
}
void Miniverse::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	
	Bool pressed = action == ActionState::Press;
	
	switch (key) {
	case Key::F: if(pressed)window.renderer.wireframeMode = !window.renderer.wireframeMode;
		break;
	case Key::W: forward.pressed = pressed;
		break;
	case Key::S: backward.pressed = pressed;
		break;
	case Key::A: left.pressed = pressed;
		break;
	case Key::D: right.pressed = pressed;
		break;
	case Key::Q: downward.pressed = pressed;
		break;
	case Key::E: upward.pressed = pressed;
		break;
	default:
		break;
	}
}