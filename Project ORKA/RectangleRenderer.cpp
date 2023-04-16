
#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

void RectangleRenderer::create(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.useMesh(resourceManager, "centeredPlane");
	rectangleMeshId = renderer.meshSystem.currentMeshID;
}

void RectangleRenderer::render(ResourceManager& resourceManager, Renderer& renderer, Vec2 pos, Vec2 size) {
	renderer.useShader(resourceManager, "color");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(pos, size));
	renderer.renderMesh(rectangleMeshId);
}
