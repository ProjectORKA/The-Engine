#include "Pong.hpp"
#include "Window.hpp"
#include "Random.hpp"

Float ballSpeedReduction = 1.5;
Float paddleSpeed        = 2.5;

void Pong::run()
{
	resourceManager.create();
	ui.create();
	ui.window("Pong", Area(1920, 1080), true, true, WindowState::Fullscreen, pongRenderer, resourceManager);
	ui.run();
}

void PongRenderer::destroy(Window& window) {}

void PongRenderer::update(Window& window)
{
	Renderer& r = window.renderer;

	const Float delta = window.renderer.deltaTime();

	for(PongPlayer& p : players) p.update(window);

	players[0].position.x = -0.9f;
	players[1].position.x = +0.9f;

	const Vec2 normalizedCursorPosition = Vec2(2) * (Vec2(window.mousePosBotLeft) / Vec2(r.getWindowSize()) - Vec2(0.5));
	const Vec3 cursorWorldPos           = inverse(r.uniforms().getPMatrix()) * Vec4(normalizedCursorPosition, 0, 1);


	for (PongBall& ball : balls) ball.update(r.time.delta, players);

	// players[0].ballLocationInput(balls);
	players[0].keyboardInput(window, W, S, D);
	// players[0].mouseInput(cursorWorldPos);
	// players[0].aiInput(balls, renderer.renderTime.delta);

	// players[1].ballLocationInput(balls);
	players[1].keyboardInput(window, UP, DOWN, LEFT);
	// players[1].aiInput(balls, r.time.delta);
	// players[1].mouseInput(cursorWorldPos);

}

void PongRenderer::connect(GameSimulation& simulation) {}

void PongRenderer::inputEvent(Window& window, InputEvent input) {}

void PongRenderer::create(ResourceManager& resourceManager, Window& window)
{
	while(balls.size() < ballCount) balls.emplace_back();
}

void PongRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& r = window.renderer;

	r.clearBackground(Color(0, 0, 0, 1));

	r.aspectCorrectNormalizedSpace();
	r.useShader(resourceManager, "color");
	r.uniforms().setCustomColor(Vec4(1));

	// paddle 1
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[0].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");
	// paddle 2
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(players[1].position, 0)), Vec3(0.01, 0.2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");
	// ball
	for(PongBall& ball : balls) ball.render(resourceManager, r);

	// walls
	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, 1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(0, -1, 0)), Vec3(2, 0.01, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	r.uniforms().setMMatrix(scale(translate(Matrix(1), Vec3(-1, 0, 0)), Vec3(0.01, 2, 0.5)));
	r.renderMesh(resourceManager, "centeredCube");

	const Float height = r.getWindowHeight();
	const Float width  = r.getWindowWidth();

	// text
	r.uniforms().setMMatrix(Matrix(1));
	r.screenSpace();
	r.fill(Color(1));
	r.textRenderSystem.setSize(50);
	r.textRenderSystem.setLetterSpacing(0.6f);
	r.textRenderSystem.alignText(Alignment::left, Alignment::top);

	r.textRenderSystem.render(resourceManager, r, toString(players[0].score), Vec2(10, height - 100));
	//r.textRenderSystem.render(resourceManager, r, toString(players[1].difficulty), Vec2(10, height - 200));
	r.textRenderSystem.alignText(Alignment::right, Alignment::top);
	r.textRenderSystem.render(resourceManager, r, toString(players[1].score), Vec2(width - 100, height - 100));
}

void PongRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
