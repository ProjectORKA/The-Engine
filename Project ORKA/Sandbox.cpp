
#include "Sandbox.hpp"
#include "Random.hpp"
#include "Window.hpp"
#include "Input.hpp"

void SandboxRenderer::create(Window& window) {
	
}
void SandboxRenderer::update(Window& window) {
	player.update(window);
}
void SandboxRenderer::render(Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;
	
	mutex.lock();

	renderer.setWireframeMode(wireframeMode);
	renderer.setCulling(false);
	renderer.setDepthTest(true);
	renderer.clearDepth();

	player.render(window);
	renderer.uniforms().mMatrix() = Matrix(1);
	renderer.shaderSystem.use("debug");

	//////////////////////////////////////////////////////////////////////////////

	player.camera.render(renderer);

	renderer.useShader("normals");
	renderer.renderMesh("sphere");

	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(String(toString(1.0f / renderer.time.delta)), Vec2(50), fonts.heading);

	////////////////////////

	mutex.unlock();
}

void SandboxRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == toggleWireframe) wireframeMode = !wireframeMode;
	player.inputEvent(window, input);
}

void Sandbox::run() {

	SandboxRenderer app;

	Window& win = window(
		"ORKA",
		Area(1920, 1080),
		WindowDecoration::decorated,
		WindowState::windowed
	);

	app.create(win);


	win.content = &app;

	ui.run();
}
