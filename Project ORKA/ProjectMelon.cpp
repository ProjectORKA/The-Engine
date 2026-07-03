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
	ui.capture(*this);
}

void MelonRenderer::inputEvent(Window& window, InputEvent& input)
{
	const Bool altPressed = window.pressed(window.altKey);

	if(input == toggleWireframe)
	{
		window.renderer.wireframeMode = !window.renderer.wireframeMode;
		input.consume();
	}
	// Keep Alt+Enter available for fullscreen toggle.
	if(input == captureMouse && !altPressed)
	{
		ui.capture(*this);
		input.consume();
	}
	player.inputEvent(window, input);
}

void MelonRenderer::renderScene(Window& window)
{
	Renderer& r = window.renderer;

	framebuffer.clear(r);

	r.uniforms().reset();

	r.setWireframeMode(r.wireframeMode);
	r.setCulling(true);
	r.setDepthTest(true);

	r.fill(Color(1, 0, 0, 1));
	r.uniforms().setSunDir(Vec4(normalize(Vec3(-0.666, 0.333, 1)), 1));

	r.useShader("MelonUberShader");
	player.render(window);

	//ui
	r.uniforms().setMMatrix(Matrix(1));
	r.setCulling(false);
	r.setDepthTest(false);
	r.setAlphaBlending(true);
	r.screenSpaceFromBottomLeft();
	r.fontSize(20.0f);
	r.letterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::bottom);
	r.text("FPS: " + toString(1 / r.time.getDelta()), Vec2(30, 30));
	r.text("player Position: " + toString(player.position), Vec2(30, 60));
	r.text("Camera Position: " + toString(player.camera.getPosition()), Vec2(30, 90));
	r.text("Press Enter to capture mouse | RMB to release", Vec2(30, 120));
	r.text("Press F for wireframe mode", Vec2(30, 150));
}

void MelonRenderer::renderInteractive(Window& window) {}
