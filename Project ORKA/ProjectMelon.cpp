#include "ProjectMelon.hpp"
#include "Window.hpp"
#include "Random.hpp"

void MelonRenderer::destroy(Window& window)
{
	GameRenderer::destroy(window);
}

void MelonRenderer::connect(GameSimulation& simulation) {}

void MelonRenderer::update(Window& window)
{
	player.update(window);
	//world.update(player);
}

void MelonRenderer::create(Window& window)
{
	GameRenderer::create(window);
}

void MelonRenderer::inputEvent(Window& window, const InputEvent input)
{
	if(input == enter) window.captureCursor();
	if(input == exit) window.releaseCursor();

	player.inputEvent(window, input);
}

void MelonRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.uniforms().reset();

	r.setWireframeMode(false);
	r.setCulling(true);
	r.setDepthTest(true);

	// setup
	// background
	//r.clearBackground(Color(0.25, 0.321, 0.0001, 1));
	//r.clearBackground(Color(0, 0, 0, 1));

	r.fill(Color(1, 0, 0, 1));
	r.uniforms().setSunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));

	r.useShader("MelonUberShader");
	player.render(window);

	// r.uniforms().mMatrix(matrixFromSize(Vec3(100,100,1)));
	// r.renderMesh("melonRock");

	//world.render(r);

	////renderer.clearBackground(Vec4(0));

	////render scene

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh("centeredCube");

	//renderer.uniforms().mMatrix(Matrix(1));
	//renderer.renderMesh("monkey");

	//ui
	r.uniforms().setMMatrix(Matrix(1));
	r.setDepthTest(false);
	r.screenSpaceFromBottomLeft();
	r.fontSize(20.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("FPS: " + toString(1 / r.time.getDelta()), Vec2(30, 30));
	r.text("player Position: " + toString(player.position), Vec2(30, 60));
	r.text("Camera Position: " + toString(player.camera.getPosition()), Vec2(30, 90));
}

void MelonRenderer::renderInteractive(Window& window) {}
