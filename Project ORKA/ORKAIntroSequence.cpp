#include "ORKAIntroSequence.hpp"
#include "Window.hpp"
#include "Camera.hpp"

void ORKAIntroSequence::init(GameRenderer& game)
{
	gameStartingAfterIntro = &game;
}

void ORKAIntroSequence::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	window.unDecorateWindow();

	renderer.clearColor(Color(0));
	renderer.clearDepth();

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	renderer.setWireframeMode(false);

	//camera stuff
	Camera c;
	c.location = Vec3(0, -15, 0);
	c.render(renderer);

	//logo stuff
	Float animationLength = 1.1f;
	Float size = min(pow(1.0f + 1.0f / pow(animationLength, 5.0f), pow(renderer.time.total - 2.0f * animationLength, 5.0f)), 100.0f);
	Float tint = max(2.0f - size, 0.0f);
	renderer.uniforms().customColor() = Color(tint, tint, tint, 1.0f);
	Matrix modelMatrix = scale(Matrix(1.0f), Vec3(size, 1.0f, size));
	modelMatrix = rotate(modelMatrix, 20.0f / (pow(renderer.time.total, 4.0f) + 1.0f), Vec3(0.0f, 0.0f, 1.0f));
	renderer.uniforms().mMatrix() = modelMatrix;

	renderer.useShader(resourceManager, "unlit");
	renderer.useTexture(resourceManager, "ProjectORKABakedLogo");
	renderer.renderMesh(resourceManager, "projectORKALogo");

	renderer.postProcess(resourceManager, "orkaIntroSequence");

	//start the time when we actually start rendering
	if(static Bool f = true)
	{
		renderer.time.reset();
		f = false;
	}

	//swap the intro scene with the followup
	if((renderer.time.total > 5))
	{
		window.content = gameStartingAfterIntro;
		window.decorateWindow();
	}
}