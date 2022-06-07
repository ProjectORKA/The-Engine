
#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

void RectangleRenderer::create(Renderer& renderer) {
	renderer.useMesh("centeredPlane");
	rectangleMeshId = renderer.meshSystem.currentMeshID;
}

void RectangleRenderer::render(Renderer& renderer, Vec2 pos, Vec2 size) {
	renderer.useShader("color");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(pos, size));
	renderer.renderMesh(rectangleMeshId);
}
