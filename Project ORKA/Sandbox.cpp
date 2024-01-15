#include "Sandbox.hpp"
#include "UserInterface.hpp"

void SandboxRenderer::update(Window& window)
{
	player.update(window);
}

void SandboxRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void SandboxRenderer::connect(GameSimulation& simulation) {}

void SandboxRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void SandboxRenderer::create(Window& window)
{
	player.camera.setLocation(Vec3(0.0f, -5.0f, 0.0f));

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();
}

void SandboxRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// render scene
	player.render(window); // sets the location, rotation and projection
	r.useShader("normals"); // sets the color / material for the rendered objects
	r.uniforms().setMMatrix(matrixFromRotation(0, 0, r.time.total)); // sets the objects transformation within the world
	r.renderMesh("suzanne"); // renders the objects 3D data to the screen

	// text rendering
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.textRenderSystem.render(r, "W A S D Q E to move", Vec2(50, 250));
	r.textRenderSystem.render(r, "F for wireframe mode", Vec2(50, 200));
	r.textRenderSystem.render(r, "Scroll to change speed", Vec2(50, 150));
	r.textRenderSystem.render(r, "FPS: " + toString(static_cast<Int>(1.0f / r.time.delta)), Vec2(50));

	// render to window
	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader("final");
}

void SandboxRenderer::renderInteractive(Window& window, TiledRectangle area) {}
