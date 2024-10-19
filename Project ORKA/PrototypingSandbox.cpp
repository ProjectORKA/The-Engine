#include "PrototypingSandbox.hpp"
#include "PerlinNoise.hpp"
#include "UserInterface.hpp"

void PrototypingRenderer::update(Window& window)
{
	player.update(window);

	if(window.isKeyPressed(Space)) prototype.action();
}

void PrototypingRenderer::destroy(Window& window)
{
	framebuffer.destroy(window.renderer);
}

void PrototypingRenderer::connect(GameSimulation& simulation) {}

void PrototypingRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if(input == action) prototype.action();

	player.inputEvent(window, input);
	prototype.inputEvent(window, input);
}

void PrototypingRenderer::create(Window& window)
{
	player.camera.setPosition(Vec3(0.0f, 0.0f, 10.0f));

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(window.renderer, WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(window.renderer, WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();

	prototype.create();
}

void PrototypingRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void PrototypingRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.setWireframeMode();
	r.setCulling(false);
	r.setDepthTest(true);

	framebuffer.resize(window.renderer, area.size);
	framebuffer.clear();
	framebuffer.bindDraw(r);

	// render scene
	player.render(window); // sets the position, rotation and projection

	if(wireframeCube)
	{
		r.useShader("debug");
		r.fill(1);
		r.wireframeCubeCentered(10.0f);
	}

	prototype.render(r, player);

	// text rendering
	r.fill(1);
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.textRenderSystem.render(r, "W A S D Q E to move", Vec2(50, 250));
	r.textRenderSystem.render(r, "F for wireframe mode", Vec2(50, 200));
	r.textRenderSystem.render(r, "Scroll to change speed", Vec2(50, 150));
	r.textRenderSystem.render(r, "FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta())), Vec2(50));

	// render to window
	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");
}
