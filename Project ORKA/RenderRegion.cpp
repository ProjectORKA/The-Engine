#include "RenderRegion.hpp"
#include "GraphicsAPI.hpp"
#include "Renderer.hpp"

RenderRegion::RenderRegion() = default;

void RenderRegion::set(Renderer& renderer, const IVec2& area)
{
	size = area;
	if (size.x <= 0) size.x = 1;
	if (size.y <= 0) size.y = 1;
	OpenGL::apiViewport(renderer.openGlContext, position.x, position.y, size.x, size.y);
	OpenGL::apiScissor(renderer.openGlContext, position.x, position.y, size.x, size.y);
}

Float RenderRegion::getAspectRatio() const
{
	if (size.x == 0 | size.y == 0) return 1.0; // prevents divide by zero
	return static_cast<Float>(size.x) / static_cast<Float>(size.y);
}

Int RenderRegion::getWidth() const
{
	return size.x;
}

Int RenderRegion::getHeight() const
{
	return size.y;
}

RenderRegion::RenderRegion(const Area area)
{
	size = area;
}

void RenderRegion::set(Renderer& renderer, const TiledRectangle& newRegion)
{
	size     = newRegion.size;
	position = newRegion.position;

	if (size.x <= 0) size.x = 1;
	if (size.y <= 0) size.y = 1;
	OpenGL::apiViewport(renderer.openGlContext, position.x, position.y, size.x, size.y);
	OpenGL::apiScissor(renderer.openGlContext, position.x, position.y, size.x, size.y);
}
