#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"

void MelonRenderer::inputEvent(Window& window, const InputEvent input) {
	if (input == enter) window.captureCursor();
	if (input == exit) window.unCaptureCursor();

	player.inputEvent(window, input);
}

void MelonRenderer::update(Window& window) {
	player.update(window);
	world.update(player);
}

void MelonRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& r = window.renderer;

	r.draw("main");
	r.clearColor(Color(Vec3(1), 1.0));
	r.clearDepth();
	r.uniforms().reset();

	r.setWireframeMode(false);
	r.setCulling(true);
	r.setDepthTest(true);

	////setup
	////background
	r.clearColor(Vec4(0.25, 0.321, 0.0001, 0));

	r.fill(Color(1, 0, 0, 1));
	r.uniforms().sunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));

	r.useShader(resourceManager, "MelonUberShader");
	player.render(resourceManager, window);

	//r.uniforms().mMatrix(matrixFromScale(Vec3(100,100,1)));
	//r.renderMesh(resourceManager, "melonRock");

	world.render(resourceManager, r);


	////renderer.clearColor(Vec4(0));

	////render scene

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh(resourceManager, "centeredCube");

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh(resourceManager, "monkey");

	//
	////ui
	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.setDepthTest(false);
	//renderer.screenSpace();
	//renderer.textRenderSystem.alignText(Alignment::end, Alignment::start);
	//renderer.textRenderSystem.render(resourceManager,renderer, toString(1 / renderer.time.delta), Vec2(30));
}

void ProjectMelon::run() {
	ui.window("Project Melon", Area(1920, 1080), true, WindowState::Windowed, game, resourceManager);
	ui.run();
}
