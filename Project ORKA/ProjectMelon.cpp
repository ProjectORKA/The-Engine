
#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"
#include "Algorithm.hpp"

void MelonRenderer::update(Renderer& renderer) {
	player.update(renderer);
}
void MelonRenderer::mouseIsMoving(Window& window, IVec2 position) {
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
void MelonRenderer::render(TiledRectangle area, Renderer& renderer) {
	//setup
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	//background
	//renderer.clearColor(Vec4(0.25, 0.321, 0.0001, 0));

	renderer.clearColor(Vec4(0));

	//scene
	player.render(renderer);

	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh("centeredCube");

	//world.render(renderer);

	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(toString(1 / renderer.time.delta), Vec2(30), fonts.heading);
}
void MelonRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.zoomFactor -= yAxis;
	player.zoomFactor = clamp(player.zoomFactor, -3, 30);
}
void MelonRenderer::buttonIsPressed(Window& window, Key key, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case Key::F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		default:
			break;
		}
	}
}
void MelonRenderer::mouseIsPressed(Window& window, MouseButton button, Int action, Int modifiers) {
	if (button == MouseButton::LEFT && action == GLFW_PRESS) {
		holdPosition.pressed = true;
		inputManager.captureCursor(window);
	}
	else holdPosition.pressed = false;
	if (button == MouseButton::RIGHT && action == GLFW_PRESS) inputManager.uncaptureCursor(window);
}