#include "TiledMath.hpp"
#include "Math.hpp"


Area::Area(const Int a) {
	x = a;
	y = a;
}

Area Area::center() const { return Area(x / 2, y / 2); }
void logDebug(const Area t) { std::cout << "(" << t.x << "|" << t.y << ")" << "\n"; }

Area::Area(const Int x, const Int y) {
	this->x = x;
	this->y = y;
}

void Area::setMinimum(const Int a) {
	x = max(x, a);
	y = max(y, a);
}

Area Area::operator*(const Float value) const {
	return Area(static_cast<Int>(static_cast<Float>(x) * value), static_cast<Int>(static_cast<Float>(y) * value));
};

void Area::setMinimum(const Area minimumSize) {
	x = max<Int>(x, minimumSize.x);
	y = max<Int>(y, minimumSize.y);
}

IVec2 TiledRectangle::center() const { return position + size / 2; }

TiledRectangle::TiledRectangle(const Area area) {
	this->size = area;
	this->position = IVec2(0);
}

TiledRectangle TiledRectangle::operator*(const Float value) const {
	TiledRectangle t;
	t.size.x = static_cast<Int>(static_cast<Float>(size.x) * value);
	t.size.y = static_cast<Int>(static_cast<Float>(size.y) * value);
	t.position.x = static_cast<Int>(static_cast<Float>(position.x) + (static_cast<Float>(size.x) * (1.0f - value) /
		2.0f));
	t.position.y = static_cast<Int>(static_cast<Float>(position.y) + (static_cast<Float>(size.y) * (1.0f - value) /
		2.0f));
	return t;
};

Bool TiledRectangle::positionInsideArea(IVec2 position) const {
	position -= this->position;
	if (position.x < 0 || position.y < 0) return false;
	return position.x <= size.x && position.y <= size.y;
}

TiledRectangle::TiledRectangle(const Int x, const Int y, const Int w, const Int h) {
	position.x = x;
	position.y = y;
	size.x = w;
	size.y = h;
}
