
#include "Sandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

void Sandbox::update(Renderer& renderer) {

	player.speed = pow(1.2, player.speedExponent);

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer);

	noct.update(player.camera.location);
}

void Sandbox::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void Sandbox::render(TiledRectangle area, Renderer& renderer) {
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(renderer); //[TODO] or this
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.shaderSystem.use("debug");

	//////////////////////////////////////////////////////////////////////////////

	

	noct.render(renderer);

	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}
void Sandbox::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void Sandbox::buttonIsPressed(Window& window, Key keyID, ActionState action, Int modifiers) {
	Bool pressed = (action != ActionState::Release);
	switch (keyID) {
	case Key::F: if (pressed) window.renderer.wireframeMode = !window.renderer.wireframeMode;
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
void Sandbox::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::LEFT && action == ActionState::Press) inputManager.captureCursor(window);
	if (button == MouseButton::RIGHT && action == ActionState::Press) inputManager.uncaptureCursor(window);
}