#include "PrototypingSandbox.hpp"
#include "PerlinNoise.hpp"
#include "UserInterface.hpp"

void PrototypingRenderer::update(Window& window)
{
	player.update(window);

	if (window.isKeyPressed(Space) || actionToggle) prototype.action();
}

void PrototypingRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void PrototypingRenderer::connect(GameSimulation& simulation) {}

void PrototypingRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;
	if (input == action) prototype.action();
	if (input == toggleAction) actionToggle = !actionToggle;

	player.inputEvent(window, input);
	prototype.inputEvent(window, input);
}

void PrototypingRenderer::create(Window& window)
{
	player.camera.setPosition(Vec3(0.0f, -3.0f, 1.0f));
	player.camera.setNearClipValue(0.001f);
	player.camera.setFarClipValue(100.00f);

	GameRenderer::create(window);

	prototype.create(window.renderer);
}

void PrototypingRenderer::renderInteractive(Window& window) {}

void PrototypingRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(window.renderer);

	r.setWireframeMode();
	r.setCulling(false);
	r.setDepthTest(true);


	// render scene
	player.render(window); // sets the position, rotation and projection

	if (wireframeCube)
	{
		r.useShader("debug");
		r.fill(1);
		r.wireframeCubeCentered(10.0f);
	}

	prototype.render(r, player);

	// text rendering
	r.fill(1);
	r.setDepthTest(false); // disables depth to always draw on top
	r.screenSpaceFromBottomLeft();       // aligns coordinate system with screenspace
	r.uniforms().setMMatrix(Matrix(1));
	r.fontSize(16.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("W A S D Q E to move", Vec2(50, 250));
	r.text("F for wireframe mode", Vec2(50, 200));
	r.text("Scroll to change speed", Vec2(50, 150));
	r.text("FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta())), Vec2(50));
}
