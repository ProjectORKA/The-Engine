#include "RectangleRenderer.hpp"
#include "Renderer.hpp"

//void RectangleRenderer::create(Renderer& renderer)
//{
//	renderer.useMesh("centeredPlane");
//	rectangleMeshId = renderer.meshSystem.currentMeshId;
//}
//
//void RectangleRenderer::render(Renderer& renderer, const Vec2 pos, const Vec2 size, const Bool centered) const
//{
//	if(centered) renderer.uniforms().setMMatrix(matrixFromPositionAndSize(pos, Vec2(size.x, -size.y)));
//	else renderer.uniforms().setMMatrix(matrixFromPositionAndSize(pos+size/Vec2(2), size));
//
//	renderer.renderMesh(rectangleMeshId);
//}
