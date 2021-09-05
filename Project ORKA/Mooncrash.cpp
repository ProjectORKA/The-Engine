#include "Mooncrash.hpp"

void Mooncrash::update() {
	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);

	if (inputManager.capturing)camera.rotate(Vec2(inputManager.cursorPosition) * inputManager.mouseSensitivity);

	camera.speedMultiplier = inputManager.scrollAxisYTotal;
	if (inputManager.forward.isPressed)	camera.accelerationVector += camera.forwardVector;
	if (inputManager.backward.isPressed) camera.accelerationVector -= camera.forwardVector;
	if (inputManager.up.isPressed) camera.accelerationVector += camera.upVector;
	if (inputManager.down.isPressed) camera.accelerationVector -= camera.upVector;
	if (inputManager.right.isPressed) camera.accelerationVector += camera.rightVector;
	if (inputManager.left.isPressed) camera.accelerationVector -= camera.rightVector;
}

void Mooncrash::render(Window& window) {
	Renderer& renderer = window.renderer;

	camera.update(renderer.renderTime.delta);

	//enables or disables wireframe mode
	renderer.setWireframeMode(renderer.wireframeMode);

	//render sky
	renderer.setCulling(false);
	renderer.setDepthTest(false);

	renderer.shaderSystem.uniforms.data.vpMatrix = camera.projectionMatrix(renderer.renderRegion.getAspectRatio()) * camera.viewMatrixOnlyRot();
	renderer.shaderSystem.uniforms.update();

	renderer.useShader("sky");
	renderer.useTexture("sky");
	renderer.renderMesh("sky");

	//render planet

	renderer.planetRenderSystem.render(gameSimulation.planetSystem, renderer, camera);
	//renderer.clearDepth();

	//renderer.shaderSystem.uniforms.data.chunkOffsetVector = Vec4((Vec3(planetCamera.chunkLocation) + planetCamera.location) / Vec3(ULLONG_MAX) ,1);
	//renderer.shaderSystem.uniforms.update();
	//renderer.sdfTerrainRenderSystem.render(renderer);

	//render text
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
