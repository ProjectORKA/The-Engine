#include "MindMapper.hpp"

void MindMapper::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA MindMapper", Area(1920, 1080), true, true, WindowState::Windowed, mindMapperRenderer, resourceManager);
	ui.run();
}

void MindMapperRenderer::destroy(Window& window) {}

void MindMapperRenderer::connect(GameSimulation& simulation) {}

void MindMapperRenderer::update(Window& window)
{
	player.update(window);

	if(grow) system.addNode();

	for(Int i = 0; i < 10; i++) system.update();
}

void MindMapperRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == growToggle) grow = !grow;
	if(input == exit) window.releaseCursor();
	if(input == enter) window.captureCursor();
	player.inputEvent(window, input);
}

void MindMapperRenderer::create(ResourceManager& resourceManager, Window& window)
{
	player.camera.setLocation(0.0f, 0.0f, 150.0f);
	player.camera.setRotation(DVec3(PI, 0, 0));

	for(Int i = 0; i < 10; i++) system.addNode();
}

void MindMapperRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(0, 0, 0, 1));

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	player.render(resourceManager, window);
	r.uniforms().setMMatrix(Matrix(1));
	r.shaderSystem.use(resourceManager, "debug");

	system.render(resourceManager, r);
}

void MindMapperRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
