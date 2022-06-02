
#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

void RectangleRenderer::render(Renderer& renderer, Float x, Float y, Float w, Float h) {
	renderer.screenSpace();
	Matrix m = matrixFromLocationAndSize2D(x, y, w, h);
	renderer.useShader("color");
	renderer.renderMesh("plane");
}
