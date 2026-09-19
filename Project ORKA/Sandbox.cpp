#include "Sandbox.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

void SandboxRenderer::update(Window& window)
{
	player.update(window);
}

void SandboxRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void SandboxRenderer::connect(GameSimulation& simulation) {}

void SandboxRenderer::inputEvent(Window& window, InputEvent& input)
{
	if (input == enter) ui.capture(*this);
	if (input == exit) ui.releaseCapture();
	if (input == wireframeToggle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void SandboxRenderer::create(Window& window)
{
	setName("SandboxRenderer");
	player.camera.setPosition(Vec3(0.0f, -5.0f, 0.0f));
	GameRenderer::create(window);
}

void SandboxRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;
	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);

	framebuffer.clearColor(r, Color(0.1, 0.1, 0.1, 1));
	framebuffer.clearDepth(r);

	//// render scene
	player.render(window);                                                // sets the position, rotation and projection
	r.useShader("normals");                                               // sets the color / material for the rendered objects
	r.uniforms().setMMatrix(matrixFromRotation(0, 0, r.time.getTotal())); // sets the objects transformation within the world
	r.renderMesh("suzanne");                                              // renders the objects 3D data to the screen

	// text rendering
	r.screenSpaceFromBottomLeft();       // aligns coordinate system with screenspace
	r.setDepthTest(false);
	r.setCulling(false);
	r.uniforms().setMMatrix(Matrix(1));
	r.fontSize(16.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	constexpr Float textSpacing = 20;
	Int i = 1;
	r.text("FPS: " + toString(static_cast<Int>(1.0f / r.time.getDelta())), Vec2(textSpacing, textSpacing * i++));
	r.text("Scroll to change speed", Vec2(textSpacing, textSpacing * i++));
	r.text("F for wireframe mode", Vec2(textSpacing, textSpacing * i++));
	r.text("W A S D Q E to move", Vec2(textSpacing, textSpacing * i++));
}

void SandboxRenderer::renderInteractive(Window& window) {}
