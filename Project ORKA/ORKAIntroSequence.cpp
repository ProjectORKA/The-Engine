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

void ORKAIntroSequence::create(ResourceManager& resourceManager, Window& window)
{
	window.unDecorateWindow();
	window.setVisible(true);
}

void ORKAIntroSequence::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0));

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setWireframeMode(false);

	// camera stuff
	Camera c;
	c.setLocation(Vec3(0.0f, -15.0f, 0.0f));
	c.render(renderer);

	// logo stuff
	Float animationLength = 1.1f;
	Float size            = min(pow(1.0f + 1.0f / pow(animationLength, 5.0f), pow(renderer.time.total - 2.0f * animationLength, 5.0f)), 100.0f);
	Float tint            = max(2.0f - size, 0.0f);
	renderer.uniforms().setCustomColor(Color(tint, tint, tint, 1.0f));
	Matrix modelMatrix = scale(Matrix(1.0f), Vec3(size, 1.0f, size));
	modelMatrix        = rotate(modelMatrix, 20.0f / (pow(renderer.time.total, 4.0f) + 1.0f), Vec3(0.0f, 0.0f, 1.0f));
	renderer.uniforms().setMMatrix(modelMatrix);

	renderer.useShader(resourceManager, "orkaIntroSequence");
	renderer.useTexture(resourceManager, "ProjectORKABakedLogo");
	renderer.renderMesh(resourceManager, "projectORKALogo");

	// start the time when we actually start rendering
	static Bool f = true;
	if(f)
	{
		renderer.time.reset();
		f = false;
	}

	// swap the intro scene with the followup
	if(renderer.time.total > 5)
	{
		window.decorateWindow();
		// window.setFullscreen();
		window.content = gameStartingAfterIntro;
		gameStartingAfterIntro->create(resourceManager, window);
	}
}

void ORKAIntroSequence::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
