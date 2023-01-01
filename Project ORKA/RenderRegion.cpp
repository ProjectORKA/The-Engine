
#include "RenderRegion.hpp"

void RenderRegion::set(TiledRectangle region) {
	this->region = region;
	apiViewport(region.position.x, region.position.y, region.size.x, region.size.y);
	apiScissor(region.position.x, region.position.y, region.size.x, region.size.y);
}

void RenderRegion::set(Area area) {
	this->region = area;
	apiViewport(region.position.x, region.position.y, region.size.x, region.size.y);
	apiScissor(region.position.x, region.position.y, region.size.x, region.size.y);
}

Float RenderRegion::getAspectRatio() {
	if ((region.size.x == 0) | (region.size.y == 0)) return 1.0;	//prevents divide by zero
	return Float(region.size.x) / Float(region.size.y);
}
