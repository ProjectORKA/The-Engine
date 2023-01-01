
#include "Sandbox.hpp"
#include "UserInterface.hpp"

void SandboxRenderer::update(Window& window) {
	player.update(window);
}

void SandboxRenderer::inputEvent(Window& window, InputEvent input) {
	
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();
	if (input == wireframeToogle) window.renderer.wireframeMode = !window.renderer.wireframeMode;


	player.inputEvent(window, input);
}

void SandboxRenderer::render(Engine& e, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	r.draw("main");
	r.clearColor(Color(Vec3(0), 1.0));
	r.setWireframeMode();
	r.setCulling(true);
	r.setDepthTest(true);
	r.clearDepth();

	//render scene
	player.render(e, window); // sets the location, rotation and projection
	r.useShader(e, "normals"); //sets the color / material for the rendered objects
	r.uniforms().mMatrix(matrixFromRotation(0, 0, r.time.total)); //sets the objetcs transformation within the world
	r.renderMesh(e, "suzanne"); // renders the objects 3D data to the screen

	r.postProcess(e, "blur"); // applies a blur effect to the image

	//text rendering
	r.setDepthTest(false); //disables depth to always draw on top
	r.screenSpace(); // aligns coordinate system with screenspace
	r.renderText(e, String("FPS: ").append(toString(1.0f / r.time.delta)), Vec2(50), fonts.heading); // renders current framerate to the screen
}

Sandbox::Sandbox(Engine& engine) {
	Window& w = window("ORKA Sandbox", Area(1920, 1080), true, WindowState::windowed, sandboxRenderer, engine);
	ui.run();
}
