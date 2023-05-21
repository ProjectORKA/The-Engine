#include "Sandbox.hpp"
#include "UserInterface.hpp"

void SandboxRenderer::create(ResourceManager& resourceManager, Window& window) {
	player.camera.location = Vec3(0, -5, 0);
}

void SandboxRenderer::update(Window& window) { player.update(window); }

void SandboxRenderer::inputEvent(Window& window, const InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.unCaptureCursor();
	if (input == wireframeToogle) window.renderer.wireframeMode = !window.renderer.wireframeMode;

	player.inputEvent(window, input);
}

void SandboxRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	r.draw("main");
	r.clearColor(Color(Vec3(0), 1.0));
	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);
	r.clearDepth();

	//render scene
	player.render(resourceManager, window); // sets the location, rotation and projection
	r.useShader(resourceManager, "normals"); //sets the color / material for the rendered objects
	r.uniforms().mMatrix(matrixFromRotation(0, 0, r.time.total)); //sets the objetcs transformation within the world
	r.renderMesh(resourceManager, "suzanne"); // renders the objects 3D data to the screen

	//text rendering
	r.setDepthTest(false); //disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace

	r.textRenderSystem.render(resourceManager, r, String("W A S D Q E to move"), Vec2(50, 250));
	r.textRenderSystem.render(resourceManager, r, String("F for wireframe mode"), Vec2(50, 200));
	r.textRenderSystem.render(resourceManager, r, String("Scroll to change speed"), Vec2(50, 150));
	r.textRenderSystem.render(resourceManager, r,
	                          String("FPS: ").append(toString(static_cast<Int>(1.0f / r.time.delta))), Vec2(50));
	// renders current framerate to the screen
}

void Sandbox::run() {
	ui.window("ORKA Sandbox", Area(1920, 1080), true, WindowState::Maximized, sandboxRenderer, resourceManager);
	ui.run();
}
