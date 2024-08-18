#include "ORKAIntroSequence.hpp"
#include "Window.hpp"
#include "Camera.hpp"

void ORKAIntroSequence::update(Window& window) {}

void ORKAIntroSequence::destroy(Window& window) {}

void ORKAIntroSequence::connect(GameSimulation& simulation) {}

void ORKAIntroSequence::init(GameRenderer& game)
{
	gameStartingAfterIntro = &game;
}

void ORKAIntroSequence::inputEvent(Window& window, InputEvent input) {}

void ORKAIntroSequence::create(Window& window)
{
	window.unDecorateWindow();
	window.setVisible(true);
}

void ORKAIntroSequence::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0));

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setAlphaBlending(true);
	renderer.setWireframeMode(false);

	// camera stuff
	Camera c;
	c.setPosition(Vec3(0.0f, -15.0f, 0.0f));
	c.render(renderer);

	// logo stuff
	constexpr Float animationLength = 1.1f;
	const Float size            = min(pow(1.0f + 1.0f / pow(animationLength, 5.0f), pow(renderer.time.getTotal() - 2.0f * animationLength, 5.0f)), 100.0f);
	const Float tint            = max(2.0f - size, 0.0f);
	renderer.uniforms().setCustomColor(Color(tint, tint, tint, 1.0f));
	Matrix modelMatrix = scale(Matrix(1.0f), Vec3(size, 1.0f, size));
	modelMatrix        = rotate(modelMatrix, 20.0f / (pow(renderer.time.getTotal(), 4.0f) + 1.0f), Vec3(0.0f, 0.0f, 1.0f));
	renderer.uniforms().setMMatrix(modelMatrix);

	renderer.useShader("orkaIntroSequence");
	renderer.useTexture("ProjectORKABakedLogo");
	renderer.renderMesh("projectORKALogo");

	// start the time when we actually start rendering
	static Bool f = true;
	if(f)
	{
		renderer.time.reset();
		f = false;
	}

	// swap the intro scene with the followup
	if(renderer.time.getTotal() > 5)
	{
		window.decorateWindow();
		
		window.content.add(*gameStartingAfterIntro);

		window.content.drop(this);

		gameStartingAfterIntro->create(window);
	}
}

void ORKAIntroSequence::renderInteractive(Window& window, TiledRectangle area) {}
