
#include "TiledMath.hpp"

IVec2 TiledRectangle::center()
{
	return position + size / 2;
}

Bool TiledRectangle::positionInsideArea(IVec2 position)
{
	position -= this->position;
	if (position.x < 0 | position.y < 0) return 0;
	return position.x <= size.x & position.y <= size.y;
}

Area center(Area area) {
	return area / 2;
}
