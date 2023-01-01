
#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

void RectangleRenderer::create(Engine& engine, Renderer& renderer) {
	renderer.useMesh(engine, "centeredPlane");
	rectangleMeshId = renderer.meshSystem.currentMeshID;
}

void RectangleRenderer::render(Engine& engine, Renderer& renderer, Vec2 pos, Vec2 size) {
	renderer.useShader(engine, "color");
	renderer.uniforms().mMatrix(matrixFromLocationAndSize2D(pos, size));
	renderer.renderMesh(rectangleMeshId);
}
