
#include "TiledMath.hpp"
#include "Math.hpp"

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

Area::Area(Int a) {
	x = a;
	y = a;
}

Area::Area(Int x, Int y) {
	this->x = x;
	this->y = y;
}

Area Area::center()
{
	return Area(x / 2, y/2);
}

void Area::clamp(Area minimumSize)
{
	x = max<Int>(x, minimumSize.x);
	y = max<Int>(y, minimumSize.y);
}

void Area::clamp(Int a) {
	x = max<Int>(x, a);
	y = max<Int>(y, a);
}

void logDebug(Area t)
{
	std::cout << "(" << t.x << "|" << t.y << ")" << "\n";
}
