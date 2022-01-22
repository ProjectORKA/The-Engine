
#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "Algorithm.hpp"

void MelonGame::update() {
	player.update();
	world.update(player);
}
void MelonGame::render(Renderer& renderer) {
	//setup
	renderer.setDepthTest(true);

	//background
	renderer.clearColor(Vec4(0.25, 0.321, 0.0001, 1));

	//scene
	player.render(renderer);

	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh("centeredCube");

	world.render(renderer);

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(toString(1 / renderer.time.delta), Vec2(30), fonts.heading);
}
void MelonGame::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window)) {

		Vec3 delta = Vec3(Vec2(Vec2(position.x, -position.y) * Vec2(mouseSensitivity)) * (player.camera.location.z + 1.0f), 0);

		if (holdPosition.pressed) {
			player.velocity += delta;
		}
		else {
			player.velocity = Vec3(0);
			player.location += delta;
		}
	}
}
void MelonGame::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.zoomFactor -= yAxis;
	player.zoomFactor = clamp(player.zoomFactor, -3, 30);
}
void MelonGame::mouseIsPressed(Window& window, Int button, Int action, Int modifiers) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		holdPosition.pressed = true;
		inputManager.captureCursor(window);
	}
	else holdPosition.pressed = false;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
}
void MelonGame::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {


		switch (keyID) {
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		default:
			break;
		}
	}
}