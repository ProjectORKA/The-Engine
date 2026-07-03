#include "MindMapper.hpp"

void MindMapperSim::destroy() {}

void MindMapperSim::update(Float delta)
{
	system.update();
}

void MindMapperRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

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

void MindMapperRenderer::inputEvent(Window& window, InputEvent& input)
{
	if(input == growToggle) grow = !grow;
	if(input == enter) ui.capture(*this);
	if(input == exit) ui.releaseCapture();
	player.inputEvent(window, input);
}

void MindMapperRenderer::create(Window& window)
{
	GameRenderer::create(window);
	player.camera.setPosition(0.0f, 0.0f, 150.0f);
	player.camera.setRotation(DVec3(PI, 0, 0));
}

void MindMapperRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(0, 0, 0, 1));

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	player.render(window);
	r.uniforms().setMMatrix(Matrix(1));
	r.shaderSystem.use(r, "debug");

	sim->system.render(r);
}

void MindMapperRenderer::renderInteractive(Window& window) {}
