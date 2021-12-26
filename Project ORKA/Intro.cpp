#include "Intro.hpp"

#include "GameSystem.hpp"

void Intro::update()
{
}

void Intro::render(Renderer& renderer)
{
	//renderer.time.update();
	renderer.clearColor(Color(0));
	renderer.clearDepth();

	renderer.setCulling(true);
	renderer.setDepthTest(true);
	//renderer.setAlphaBlending(false);
	renderer.setWireframeMode(false);

	//camera stuff
	Camera c;
	c.location = Vec3(0, -15, 0);
	c.render(renderer);

	//logo stuff
	Float animationLength = 1.1;
	Float size = min(pow(1 + 1 / pow(animationLength, 5), pow(renderer.renderTime.total - 2 * animationLength, 5)), 100.0);
	Float tint = max(2 - size, 0.0f);
	renderer.uniforms().data.customColor = Color(tint, tint, tint, 1);
	Matrix modelMatrix = glm::scale(Matrix(1), Vec3(size, 1, size));
	modelMatrix = glm::rotate(modelMatrix, 20 / (pow(renderer.renderTime.total, 4.0f) + 1), Vec3(0, 0, 1));
	renderer.uniforms().data.mMatrix = modelMatrix;

	renderer.useShader("unlit");
	renderer.useTexture("ProjectORKABakedLogo");
	renderer.renderMesh("ProjectORKALogo");

	static Bool f = true;
	if (f) {
		renderer.renderTime.reset();
		f = false;
	}


	if ((renderer.renderTime.total > 5) && (!selfReplace)) {
		selfReplace = gameStartingAfterIntro;
	}
}