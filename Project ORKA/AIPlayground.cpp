#include "AIPlayground.hpp"
#include "PerlinNoise.hpp"
#include "Window.hpp"

void AIPlaygroundRenderer::update(Window& window)
{
	player.update(window);
}

void AIPlaygroundRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void AIPlaygroundRenderer::connect(GameSimulation& simulation) {}

void AIPlaygroundRenderer::create(Window& window)
{
	GameRenderer::create(window);
}

void AIPlaygroundRenderer::renderInteractive(Window& window) {}

void AIPlaygroundRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.clear(window.renderer);
	r.setWireframeMode();
	r.useShader("color");

	player.render(window);

	//render scene

	//render text
	r.setWireframeMode(false);
	r.screenSpaceFromBottomLeft();
	r.fill(Color(1));
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.alignText(Alignment::left, Alignment::center);
	r.textRenderSystem.render(r, String(toString(1.0f / r.time.getDelta())), Vec2(50));
}

void AIPlaygroundRenderer::inputEvent(Window& window, const InputEvent input)
{
	if (input == enter) window.captureCursor();
	if (input == exit) window.releaseCursor();
	if (input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}
