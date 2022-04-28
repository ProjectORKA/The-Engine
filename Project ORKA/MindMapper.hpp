
#pragma once

#include "Game.hpp"
#include "MindMap.hpp"
#include "Player.hpp"

struct MindMapper : public GameRenderer {

	Mutex mutex;
	Player player;
	Action forward;
	Action backward;
	Action left;
	Action right;
	Action upward;
	Action downward;
	Float mouseSensitivity = 0.0015f;

	MindMap system;

	Bool keepGrowing = false;

	void update(Renderer& renderer) override;
	void mouseIsMoving(Window& window, IVec2 position)  override;
	void render(TiledRectangle area, Renderer& renderer) override;
	void mouseIsScrolled(Window& window, Double xAxis, Double yAxis) override;
	void buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) override;
	void mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) override;
};

void MindMapper::update(Renderer& renderer) {

	player.speed = pow(1.2, player.speedExponent);

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.update(renderer);

	if (keepGrowing) system.addNode();
	system.update();
}
void MindMapper::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void MindMapper::render(TiledRectangle area, Renderer& renderer) {
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(renderer); //[TODO] or this
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.shaderSystem.use("debug");

	//////////////////////////////////////////////////////////////////////////////

	system.render(renderer);

	//renderer.setDepthTest(false);
	//renderer.screenSpace();
	//renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}
void MindMapper::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.speedExponent += yAxis;
}
void MindMapper::buttonIsPressed(Window& window, Key keyID, ActionState action, Int modifiers) {
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
	case Key::SPACE: if (pressed) keepGrowing = !keepGrowing;
		break;
	default:
		break;
	}
}
void MindMapper::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	if (button == MouseButton::LEFT && action == ActionState::Press) inputManager.captureCursor(window);
	if (button == MouseButton::RIGHT && action == ActionState::Press) inputManager.uncaptureCursor(window);
}