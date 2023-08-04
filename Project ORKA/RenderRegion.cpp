#include "RenderRegion.hpp"
#include "GraphicsAPI.hpp"

RenderRegion::RenderRegion() = default;

void RenderRegion::set(const Area area)
{
	region = area;
	if(region.size.x <= 0) region.size.x = 1;
	if(region.size.y <= 0) region.size.y = 1;
	OpenGL::apiViewport(region.position.x, region.position.y, region.size.x, region.size.y);
	OpenGL::apiScissor(region.position.x, region.position.y, region.size.x, region.size.y);
}

Float RenderRegion::getAspectRatio() const
{
	if(region.size.x == 0 | region.size.y == 0) return 1.0; // prevents divide by zero
	return static_cast<Float>(region.size.x) / static_cast<Float>(region.size.y);
}

RenderRegion::RenderRegion(const Area area)
{
	region = area;
}

void RenderRegion::set(const TiledRectangle& newRegion)
{
	this->region = newRegion;
	if(region.size.x <= 0) region.size.x = 1;
	if(region.size.y <= 0) region.size.y = 1;
	OpenGL::apiViewport(region.position.x, region.position.y, region.size.x, region.size.y);
	OpenGL::apiScissor(region.position.x, region.position.y, region.size.x, region.size.y);
}
