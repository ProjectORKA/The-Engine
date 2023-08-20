#include "MSDFText.hpp"

void MSDFText::run()
{
	resourceManager.create();
	ui.create();
	ui.window("MSDF Text Rendering", Area(settings.defaultWindowWidth, settings.defaultWindowHeight), true, true, WindowState::Windowed, msdfTextRenderer, resourceManager);
	ui.run();
}

void MSDFTextRenderer::update(Window& window) {}

void MSDFTextRenderer::destroy(Window& window) {}

void MSDFTextRenderer::connect(GameSimulation& simulation) {}

void MSDFTextRenderer::inputEvent(Window& window, InputEvent input) {}

void MSDFTextRenderer::create(ResourceManager& resourceManager, Window& window) {}

void MSDFTextRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.clearBackground(Color(0, 0, 0, 1));
	renderer.aspectCorrectNormalizedSpace();

	renderer.useShader(resourceManager, "msdfFont");
	renderer.useTexture(resourceManager, "msdfFont");

	renderer.uniforms().setMMatrix(matrixFromLocationAndSize2D(Vec2(0), Vec2(2)));
	renderer.renderMesh(resourceManager, "centeredPlane");
}

void MSDFTextRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) {}
