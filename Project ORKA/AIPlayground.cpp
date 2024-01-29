#include "AIPlayground.hpp"
#include "PerlinNoise.hpp"
#include "Window.hpp"

void AIPlaygroundRenderer::update(Window& window)
{
	player.update(window);
}

void AIPlaygroundRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void AIPlaygroundRenderer::connect(GameSimulation& simulation) {}

void AIPlaygroundRenderer::create(Window& window)
{
	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();
}

void AIPlaygroundRenderer::renderInteractive(Window& window, TiledRectangle area) { }

void AIPlaygroundRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();
	r.setWireframeMode();
	r.useShader("color");

	player.render(window);

	//render scene
	network.render(r);

	//render text
	r.setWireframeMode(false);
	r.screenSpace();
	r.fill(Color(1));
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.alignText(Alignment::left, Alignment::center);
	r.textRenderSystem.render(r, String(toString(1.0f / r.time.getDelta())), Vec2(50));

	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");

	TiledRectangle topBarArea = area;
	topBarArea.size.y         = 100;

	topBar.render(window, topBarArea);
}

void AIPlaygroundRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}
