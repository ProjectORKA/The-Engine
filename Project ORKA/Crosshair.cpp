#include "Crosshair.hpp"
#include "Renderer.hpp"
#include "Window.hpp"

void renderCrosshairDot(Window& window, const Float outerSize, const Float innerSize)
{
	renderCrosshairDot(window.renderer, Vec2(window.getContentSize() / 2), outerSize, innerSize);
}

void renderCrosshairDot(Renderer& renderer, const Vec2 center, const Float outerSize, const Float innerSize)
{
	renderer.uniforms().reset();
	renderer.screenSpaceFromBottomLeft();
	renderer.useShader("color");
	renderer.fill(0.1f, 0.1f, 0.1f);
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(center, outerSize));
	renderer.renderMesh("circle");
	renderer.fill(1, 1, 1);
	renderer.uniforms().setMMatrix(matrixFromPositionAndSize(center, innerSize));
	renderer.renderMesh("circle");
}
