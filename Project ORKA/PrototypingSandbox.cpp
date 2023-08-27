#include "PrototypingSandbox.hpp"
#include "PerlinNoise.hpp"
#include "UserInterface.hpp"

void PrototypingSandbox::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Prototyping Sandbox", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, renderer, resourceManager);
	ui.run();
}

void PrototypingRenderer::update(Window& window)
{
	player.update(window);
}

void PrototypingRenderer::destroy(Window& window)
{
	framebuffer.destroy();
}

void PrototypingRenderer::connect(GameSimulation& simulation) {}

void PrototypingRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();
	if(input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if(input == action) prototype.action();

	player.inputEvent(window, input);
}

void PrototypingRenderer::create(ResourceManager& resourceManager, Window& window)
{
	player.camera.setLocation(Vec3(0.0f, -5.0f, 0.0f));

	framebuffer.create("MainFramebuffer", Area(1920, 1080));
	framebuffer.add(WritePixelsFormat::RGBA, DataType::Float, FramebufferAttachment::Color0, true, Wrapping::Clamped);
	framebuffer.add(WritePixelsFormat::Depth, DataType::Float, FramebufferAttachment::Depth, false, Wrapping::Clamped);
	framebuffer.checkComplete();

	prototype.create();
}

void PrototypingRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}

void PrototypingRenderer::render(ResourceManager& resourceManager, Window& window, const TiledRectangle area)
{
	Renderer&        r  = window.renderer;
	ResourceManager& rm = resourceManager;

	r.setWireframeMode();
	r.setCulling(false);
	r.setDepthTest(true);

	framebuffer.resize(area.size);
	framebuffer.clear();
	framebuffer.bindDraw();

	// render scene
	player.render(rm, window); // sets the location, rotation and projection

	prototype.render(r, rm, player);

	// text rendering
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.uniforms().setMMatrix(Matrix(1));
	r.textRenderSystem.setSize(16.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
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

// PROTOTYPES

void PlaneIntersectionPrototype::create() {}

void PlaneIntersectionPrototype::action() {}

void PlaneIntersectionPrototype::render(Renderer& r, ResourceManager& rm, const Player& player)
{
	const Vec3 dir = normalize(Vec3(noise.noise0_1(r.time.total) * 2 - 1, noise.noise0_1(r.time.total + 123.456) * 2 - 1, noise.noise0_1(r.time.total - 654.321) * 2 - 1));
	r.arrow(rm, Vec3(0, 0, 0), dir);
	r.useShader(rm, "normals"); // sets the color / material for the rendered objects
	r.renderMesh(rm, "centeredPlane");

	const Vec3 rayLocation  = player.camera.getLocation();
	const Vec3 rayDirection = player.camera.getForwardVector();
}
