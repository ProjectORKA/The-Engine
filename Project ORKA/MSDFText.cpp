#include "MSDFText.hpp"



void MSDFTextRenderer::update(Window& window) {}

void MSDFTextRenderer::destroy(Window& window) {}

void MSDFTextRenderer::connect(GameSimulation& simulation) {}

void MSDFTextRenderer::inputEvent(Window& window, InputEvent input) {}

void MSDFTextRenderer::create(Window& window) {}

void MSDFTextRenderer::render(Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0, 0, 0, 1));
	renderer.aspectCorrectNormalizedSpace();

	renderer.useShader("msdfFont");
	renderer.useTexture("msdfFont");

	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(0), Vec2(2)));
	renderer.renderMesh("centeredPlane");
}

void MSDFTextRenderer::renderInteractive(Window& window, TiledRectangle area) {}
