
#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"

void MelonRenderer::update(Renderer& renderer) {
	player.update(renderer);
	//world.update(player);
}
void MelonRenderer::mouseIsMoving(Window& window, IVec2 position) {
	if (inputManager.isCapturing(window)) {

		Vec3 delta = Vec3(Vec2(Vec2(position.x, -position.y) * Vec2(mouseSensitivity)) * (player.camera.location.z + 1.0f), 0);

		if (inputManager.capturing) {
			player.velocity = Vec3(0);
			player.targetLocation += delta;
		}
	}
	else {
		player.targetLocation = Vec3(position.x,-position.y,0);
	}
}
void MelonRenderer::render(TiledRectangle area, Renderer& renderer) {
	//setup
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	//background
	//renderer.clearColor(Vec4(0.25, 0.321, 0.0001, 0));

	renderer.clearColor(Vec4(0));

	//render scene
	renderer.uniforms().sunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));
	player.render(renderer);

	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh("centeredCube");

	//world.render(renderer);
	
	//bloomBuffer.resize(Area(renderer.framebufferSystem.framebufferSize.x / 2, renderer.framebufferSystem.framebufferSize.y / 2));
	//renderer.framebufferSystem.current().setAsTexture(0);
	//bloomBuffer.use();
	//renderer.useShader("bloom");
	//renderer.renderMesh("fullScreenQuad");
	
	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(toString(1 / renderer.time.delta), Vec2(30), fonts.heading);
}
void MelonRenderer::mouseIsScrolled(Window& window, Double xAxis, Double yAxis) {
	player.zoomFactor -= yAxis;
	player.zoomFactor = clamp(player.zoomFactor, -3, 30);
}
void MelonRenderer::buttonIsPressed(Window& window, Key key, ActionState action, Int modifiers) {
	if (action == ActionState::Press) {
		switch (key) {
		case Key::F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		default:
			break;
		}
	}
}
void MelonRenderer::mouseIsPressed(Window& window, MouseButton button, ActionState action, Int modifiers) {
	Bool pressed = action > ActionState::Release;
	
	switch(button) {
	case MouseButton::LEFT: if (pressed) inputManager.captureCursor(window); break;
	case MouseButton::RIGHT: if (pressed) inputManager.uncaptureCursor(window); break;
	default: break;
	}
}