#include "MindMapper.hpp"

void MindMapper::update(Window& window) {
	player.update(window);

	logDebug(player.camera.location);
	logDebug(player.camera.getRotation());

	if (grow) system.addNode();
	system.update();
}
void MindMapper::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;
	
	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(window);
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
void MindMapper::inputEvent(Window& window, InputEvent input) {
	if (input == growToggle)grow = !grow;
	if (input == exit) window.uncaptureCursor();
	if (input == enter) window.captureCursor();
	player.inputEvent(window, input);
}
void MindMapper::mouseMoved(Window& window, MouseMovementInput input) {
	player.mouseMoved(window, input);
}