
#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"

void MelonRenderer::inputEvent(Window& window, InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.uncaptureCursor();

	player.inputEvent(window, input);
}

void MelonRenderer::update(Window& window) {
	player.update(window);
	//world.update(player);
}
void MelonRenderer::render(Engine& engine, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	//setup
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	//background
	//renderer.clearColor(Vec4(0.25, 0.321, 0.0001, 0));

	//renderer.clearColor(Vec4(0));

	//render scene
	renderer.uniforms().sunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));
	player.render(engine, window);

	renderer.uniforms().mMatrix(Matrix(1));
	renderer.renderMesh(engine, "centeredCube");

	//world.render(renderer);
	
	//ui
	renderer.setDepthTest(false);
	renderer.screenSpace();
	renderer.renderText(engine, toString(1 / renderer.time.delta), Vec2(30), fonts.heading);
}
