#include "ORKAIntroSequence.hpp"
#include "Window.hpp"
#include "Camera.hpp"

void ORKAIntroSequence::init(GameRenderer& game) { gameStartingAfterIntro = &game; }

void ORKAIntroSequence::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {
	Renderer& renderer = window.renderer;

	window.undecorateWindow();

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
	Float animationLength = 1.1;
	Float size = min(pow(1 + 1 / pow(animationLength, 5), pow(renderer.time.total - 2 * animationLength, 5)), 100.0);
	Float tint = max(2 - size, 0.0f);
	renderer.uniforms().customColor() = Color(tint, tint, tint, 1);
	Matrix modelMatrix = scale(Matrix(1), Vec3(size, 1, size));
	modelMatrix = rotate(modelMatrix, 20 / (pow(renderer.time.total, 4.0f) + 1), Vec3(0, 0, 1));
	renderer.uniforms().mMatrix() = modelMatrix;

	renderer.useShader(resourceManager, "unlit");
	renderer.useTexture(resourceManager, "ProjectORKABakedLogo");
	renderer.renderMesh(resourceManager, "projectORKALogo");

	renderer.postProcess(resourceManager, "orkaIntroSequence");

	//start the time when we actually start rendering
	static Bool f = true;
	if (f) {
		renderer.time.reset();
		f = false;
	}

	//swap the intro scene with the followup
	if ((renderer.time.total > 5)) {
		window.content = gameStartingAfterIntro;
		window.decorateWindow();
	}
}
