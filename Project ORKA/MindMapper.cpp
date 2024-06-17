#include "MindMapper.hpp"

void MindMapperSim::destroy() {}

void MindMapperSim::update(Float delta)
{
	system.update();
}

void MindMapperRenderer::destroy(Window& window) {}

void MindMapperRenderer::update(Window& window)
{
	player.update(window);

	if(grow) sim->system.addNode();
}

void MindMapperRenderer::connect(GameSimulation& simulation)
{
	this->sim = static_cast<MindMapperSim*>(&simulation);
}

void MindMapperSim::create()
{
	for(Int i = 0; i < nodeCount; i++) system.addNode();
}

void MindMapperRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == growToggle) grow = !grow;
	if(input == exit) window.releaseCursor();
	if(input == enter) window.captureCursor();
	player.inputEvent(window, input);
}

void MindMapperRenderer::create(Window& window)
{
	player.camera.setPosition(0.0f, 0.0f, 150.0f);
	player.camera.setRotation(DVec3(PI, 0, 0));
}

void MindMapperRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(0, 0, 0, 1));

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	player.render(window);
	r.uniforms().setMMatrix(Matrix(1));
	r.shaderSystem.use("debug");

	sim->system.render(r);
}

void MindMapperRenderer::renderInteractive(Window& window, TiledRectangle area) {}
