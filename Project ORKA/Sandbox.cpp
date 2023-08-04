#include "Sandbox.hpp"
#include "UserInterface.hpp"

void Sandbox::run()
{
	resourceManager.create();
	ui.create();
	ui.window("ORKA Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void SandboxRenderer::update(Window& window)
{
	player.update(window);
}

void SandboxRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void SandboxRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.unCaptureCursor();
	if(input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void SandboxRenderer::create(ResourceManager& resourceManager, Window& window)
{
	player.camera.setLocation(Vec3(0.0f, -5.0f, 0.0f));

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();
}

void SandboxRenderer::render(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer&        r  = window.renderer;
	ResourceManager& rm = resourceManager;

	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// render scene
	player.render(rm, window); // sets the location, rotation and projection
	r.useShader(rm, "normals"); // sets the color / material for the rendered objects
	r.uniforms().setMMatrix(matrixFromRotation(0, 0, r.time.total)); // sets the objects transformation within the world
	r.renderMesh(rm, "suzanne"); // renders the objects 3D data to the screen

	// text rendering
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setStyle(fonts.paragraph);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.textRenderSystem.render(rm, r, "W A S D Q E to move", Vec2(50, 250));
	r.textRenderSystem.render(rm, r, "F for wireframe mode", Vec2(50, 200));
	r.textRenderSystem.render(rm, r, "Scroll to change speed", Vec2(50, 150));
	r.textRenderSystem.render(rm, r, "FPS: " + toString(static_cast<Int>(1.0f / r.time.delta)), Vec2(50));

	// render to window
	r.setDepthTest(false);
	framebuffer.setAsTexture(0);
	r.drawToWindow();
	r.fullScreenShader(resourceManager, "final");
}

void SandboxRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
