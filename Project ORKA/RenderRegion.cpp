
#include "RenderRegion.hpp"

void RenderRegion::setRenderRegion(TiledRectangle region) {
	this->region = region;
	glViewport(region.position.x, region.position.y, region.size.x, region.size.y);
	glScissor(region.position.x, region.position.y, region.size.x, region.size.y);
}

Float RenderRegion::getAspectRatio() {
	if (region.size.x == 0 | region.size.y == 0) return 1.0;	//prevents divide by zero
	return Float(region.size.x) / Float(region.size.y);
}
