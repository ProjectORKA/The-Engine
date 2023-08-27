#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"

void ProjectMelon::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Project Melon", Area(1920, 1080), true, true, WindowState::Windowed, game, resourceManager);
	ui.run();
}

void MelonRenderer::destroy(Window& window) {}

void MelonRenderer::connect(GameSimulation& simulation) {}

void MelonRenderer::update(Window& window)
{
	player.update(window);
	//world.update(player);
}

void MelonRenderer::create(ResourceManager& resourceManager, Window& window) {}

void MelonRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();

	player.inputEvent(window, input);
}

void MelonRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(0, 0, 0, 1));

	r.uniforms().reset();

	r.setWireframeMode(false);
	r.setCulling(true);
	r.setDepthTest(true);

	// setup
	// background
	//r.clearBackground(Color(0.25, 0.321, 0.0001, 1));
	r.clearBackground(Color(0, 0, 0, 1));

	r.fill(Color(1, 0, 0, 1));
	r.uniforms().setSunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));

	r.useShader(resourceManager, "MelonUberShader");
	player.render(resourceManager, window);

	// r.uniforms().mMatrix(matrixFromSize(Vec3(100,100,1)));
	// r.renderMesh(resourceManager, "melonRock");

	//world.render(resourceManager, r);

	////renderer.clearBackground(Vec4(0));

	////render scene

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh(resourceManager, "centeredCube");

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh(resourceManager, "monkey");

	//ui
	r.uniforms().setMMatrix(Matrix(1));
	r.setDepthTest(false);
	r.screenSpace();
	r.textRenderSystem.setSize(20.0f);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.textRenderSystem.render(resourceManager, r, "FPS: " + toString(1 / r.time.delta), Vec2(30, 30));
	r.textRenderSystem.render(resourceManager, r, "player Location: " + toString(player.location), Vec2(30, 60));
	r.textRenderSystem.render(resourceManager, r, "Camera Location: " + toString(player.camera.getLocation()), Vec2(30, 90));
}

void MelonRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
