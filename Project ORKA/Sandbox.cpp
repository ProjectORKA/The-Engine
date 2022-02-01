
#include "Sandbox.hpp"

#include "Window.hpp"

#include "Random.hpp"

void Sandbox::render(Renderer& renderer) {
	mutex.lock();

	player.speed = pow(1.2, player.speedExponent);

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.render(renderer); //[TODO] get rid of this vvvvvvvv

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(renderer); //[TODO] or this
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.shaderSystem.use("debug");

	//////////////////////////////////////////////////////////////////////////////

	sm.update();

	sm.render(renderer);

	//tree.update(player.camera.location);
	//tree.render(renderer);

	//dmt.update(player.camera);
	//dmt.render(renderer);

	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////


	mutex.unlock();

}
void Sandbox::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.camera.rotate(Vec2(position) * Vec2(mouseSensitivity));
}
void Sandbox::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) inputManager.captureCursor(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
}
void Sandbox::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {


		switch (keyID) {
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case GLFW_KEY_W: forward.pressed = true;
			break;
		case GLFW_KEY_S: backward.pressed = true;
			break;
		case GLFW_KEY_A: left.pressed = true;
			break;
		case GLFW_KEY_D: right.pressed = true;
			break;
		case GLFW_KEY_Q: downward.pressed = true;
			break;
		case GLFW_KEY_E: upward.pressed = true;
			break;
		case GLFW_KEY_SPACE:
		{
		}
		break;
		default:
			break;
		}
	}

	if (action == GLFW_RELEASE) {
		switch (keyID) {
		case GLFW_KEY_W: forward.pressed = false;
			break;
		case GLFW_KEY_S: backward.pressed = false;
			break;
		case GLFW_KEY_A: left.pressed = false;
			break;
		case GLFW_KEY_D: right.pressed = false;
			break;
		case GLFW_KEY_Q: downward.pressed = false;
			break;
		case GLFW_KEY_E: upward.pressed = false;
			break;
		default:
			break;
		}
	}
}