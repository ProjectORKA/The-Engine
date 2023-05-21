#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

void RectangleRenderer::create(ResourceManager& resourceManager, Renderer& renderer) {
	renderer.useMesh(resourceManager, "centeredPlane");
	rectangleMeshId = renderer.meshSystem.currentMeshId;
}

void RectangleRenderer::render(ResourceManager& resourceManager, Renderer& renderer, const Vec2 pos,
                               const Vec2 size) const {
	renderer.useShader(resourceManager, "color");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(pos, size));
	renderer.renderMesh(rectangleMeshId);
}
