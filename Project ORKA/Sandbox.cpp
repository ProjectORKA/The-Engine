
#include "Sandbox.hpp"
#include "Random.hpp"
#include "Window.hpp"

Sandbox::Sandbox(SandboxSimulation& sim) {
	simulation = &sim;
}

void Sandbox::create(Window& window) {
	//Input & input = window.input;
	////setup keymap
	//input.add("forward", ButtonType::Key, Key::W);
	//input.add("backward", ButtonType::Key, Key::S);
	//input.add("right", ButtonType::Key, Key::D);
	//input.add("left", ButtonType::Key, Key::A);
	//input.add("up", ButtonType::Key, Key::E);
	//input.add("down", ButtonType::Key, Key::Q);
}

void Sandbox::update(Window& window) {
	
	player.update(window);

	simulation->location = player.camera.location;

	//system.update();
}

void Sandbox::render(Window& window, TiledRectangle area) {
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

	player.camera.renderOnlyRot(renderer);

	//system.render(renderer, player.camera.location);

	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}

void SandboxSimulation::update(Float timestep) {
	//noct.update(location);
}
SandboxSimulation::SandboxSimulation() {
	//system.create();
}
SandboxSimulation::~SandboxSimulation() {
	//noct.destroy();
}