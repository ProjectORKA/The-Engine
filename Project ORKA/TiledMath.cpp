
#include "TiledMath.hpp"
#include "Math.hpp"

Area::Area(Int a) {
	x = a;
	y = a;
}
Area Area::center()
{
	return Area(x / 2, y/2);
}
void logDebug(Area t)
{
	std::cout << "(" << t.x << "|" << t.y << ")" << "\n";
}
Area::Area(Int x, Int y) {
	this->x = x;
	this->y = y;
}
void Area::setMinimum(Int a) {
	x = max(x, a);
	y = max(y, a);
}
IVec2 TiledRectangle::center()
{
	return position + size / 2;
}
void Area::setMinimum(Area minimumSize)
{
	x = max<Int>(x, minimumSize.x);
	y = max<Int>(y, minimumSize.y);
}
TiledRectangle::TiledRectangle(Area area){
	this->size = area;
	this->position = IVec2(0);
}
Bool TiledRectangle::positionInsideArea(IVec2 position)
{
	position -= this->position;
	if (position.x < 0 | position.y < 0) return 0;
	return position.x <= size.x & position.y <= size.y;
}
TiledRectangle::TiledRectangle(Int x, Int y, Int w, Int h) {
	position.x = x;
	position.y = y;
	size.x = w;
	size.y = h;
}