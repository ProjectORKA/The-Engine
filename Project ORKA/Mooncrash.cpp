#include "Mooncrash.hpp"

void renderUI(Renderer & renderer, PlanetCamera & camera){
	renderer.setWireframeMode(false);

	String s = String("FrameTime: ").append(toString(renderer.renderTime.delta));
	renderer.textRenderSystem.render(s, 0, 0, fonts.debug);

	s = String("FPS: ").append(toString(1.0f / renderer.renderTime.delta));
	renderer.textRenderSystem.render(s, 0, fonts.debug.absoluteSize, fonts.debug);

	s = String("Speed: ").append(toString(inputManager.scrollAxisYTotal));
	renderer.textRenderSystem.render(s, 0, 2 * fonts.debug.absoluteSize, fonts.debug);

	s = String("Camera height: ").append(toString(
		(Float(camera.chunkLocation.z) + camera.location.z) / Float(ULLONG_MAX)
	));
	renderer.textRenderSystem.render(s, 0, 3 * fonts.debug.absoluteSize, fonts.debug);

	s = String("Camera Location X: ").append(toString(
		(Float(camera.chunkLocation.x) + camera.location.x) / Float(ULLONG_MAX)
	));
	renderer.textRenderSystem.render(s, 0, 4 * fonts.debug.absoluteSize, fonts.debug);
}
void renderSky(Renderer & renderer, PlanetCamera & camera) {
	renderer.setCulling(false);
	renderer.setDepthTest(false);

	renderer.shaderSystem.uniforms.data.vpMatrix = camera.projectionMatrix(renderer.renderRegion.getAspectRatio()) * camera.viewMatrixOnlyRot();
	renderer.shaderSystem.uniforms.update();

	renderer.useShader("sky");
	renderer.useTexture("sky");
	renderer.renderMesh("sky");
}
void renderPlanet(Renderer& renderer, PlanetSystem& planetSystem, PlanetCamera& camera) {
	renderer.planetRenderSystem.render(planetSystem, renderer, camera);
};




void Mooncrash::buttonIsPressed(Window& window, Int keyID, Int action, Int modifiers) {
	if (action == GLFW_PRESS) {

		switch (keyID) {
		case GLFW_KEY_F: window.renderer.wireframeMode = !window.renderer.wireframeMode;
			break;
		case GLFW_KEY_J: window.renderer.adjustRenderVariables = !window.renderer.adjustRenderVariables;
			break;
		case GLFW_KEY_K: window.renderer.planetRenderSystem.worldDistortion = !window.renderer.planetRenderSystem.worldDistortion;
			break;
		case GLFW_KEY_G: window.renderer.planetRenderSystem.chunkBorders = !window.renderer.planetRenderSystem.chunkBorders;
			break;
		case GLFW_KEY_T: {
			window.renderer.mutex.lock();
			window.renderer.shaderSystem.rebuild();
			window.renderer.mutex.unlock();
		}
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

void Mooncrash::update() {
	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);
	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);

	camera.speedMultiplier = inputManager.scrollAxisYTotal;
	if(forward.pressed)	camera.accelerationVector += camera.forwardVector;
	if(backward.pressed) camera.accelerationVector -= camera.forwardVector;
	if(upward.pressed) camera.accelerationVector += camera.upVector;
	if(downward.pressed) camera.accelerationVector -= camera.upVector;
	if(right.pressed) camera.accelerationVector += camera.rightVector;
	if(left.pressed) camera.accelerationVector -= camera.rightVector;
}
void Mooncrash::render(Window& window) {
	Renderer& renderer = window.renderer;

	camera.update(renderer.renderTime.delta);

	renderer.setWireframeMode(renderer.wireframeMode);

	renderSky(renderer, camera);

	renderPlanet(renderer, gameSimulation.planetSystem, camera);

	renderUI(renderer, camera);
}