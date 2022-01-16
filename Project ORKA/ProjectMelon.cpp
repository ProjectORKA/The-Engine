
#include "ProjectMelon.hpp"

void ProjectMelonGame::update() {
	player.update();
}
void ProjectMelonPlayer::update() {
	smoke.update(location);
}
void ProjectMelonGame::render(Renderer& renderer) {

	if (forward.pressed)	player.accelerationVector += player.camera.forwardVector;
	if (backward.pressed)	player.accelerationVector -= player.camera.forwardVector;
	if (upward.pressed)		player.accelerationVector += player.camera.upVector;
	if (downward.pressed)	player.accelerationVector -= player.camera.upVector;
	if (right.pressed)		player.accelerationVector += player.camera.rightVector;
	if (left.pressed)		player.accelerationVector -= player.camera.rightVector;

	player.render(renderer);

	renderer.setDepthTest(true);


	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh("centeredCube");
}
void ProjectMelonPlayer::render(Renderer& renderer) {
	camera.location = location + Vec3(0, 0, pow(1.2, zoomFactor));

	camera.rotationX = -PI / 2;

	camera.rotate(Vec2(0));//[TODO] camera update instead

	camera.render(renderer);
	renderer.uniforms().mMatrix(scale(translate(Matrix(1), location), Vec3(0.5)));
	renderer.uniforms().customColor(Vec4(1));
	renderer.useShader("MelonUberShader");
	renderer.renderMesh("sphere");

	smoke.render(renderer);
}
void ProjectMelonGame::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window))player.location += Vec3(Vec2(Vec2(position.x, -position.y) * Vec2(mouseSensitivity)), 0);
}
void ProjectMelonGame::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.zoomFactor -= yAxis;
}
void ProjectMelonGame::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) inputManager.captureCursor(window);
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
}
void ProjectMelonGame::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
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