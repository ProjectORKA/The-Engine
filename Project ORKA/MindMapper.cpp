#include "MindMapper.hpp"

void MindMapperRenderer::update(Window& window) {
	player.update(window);

	logDebug(player.camera.location);
	logDebug(player.camera.getRotation());

	if (grow) system.addNode();
	system.update();
}

void MindMapperRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	mutex.lock();

	renderer.setWireframeMode();
	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(resourceManager, window);
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.shaderSystem.use(resourceManager, "debug");

	//////////////////////////////////////////////////////////////////////////////

	system.render(resourceManager, renderer);

	//renderer.setDepthTest(false);
	//renderer.screenSpace();
	//renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}

void MindMapper::run() {
	resourceManager.create();
	ui.create();
	ui.window("ORKA MindMapper", Area(1920, 1080), true, WindowState::Windowed, mindMapperRenderer, resourceManager);
	ui.run();
}

void MindMapperRenderer::inputEvent(Window& window, const InputEvent input) {
	if (input == growToggle)grow = !grow;
	if (input == exit) window.unCaptureCursor();
	if (input == enter) window.captureCursor();
	player.inputEvent(window, input);
}
