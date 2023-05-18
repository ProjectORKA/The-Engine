#include "QuadtreeID.hpp"

Bool QuadtreeID::getX() const {
	if (level == 0) return false;
	return BitSet<64>(location.x)[64 - level];
}

Bool QuadtreeID::getY() const {
	if (level == 0) return false;
	return BitSet<64>(location.y)[64 - level];
}

void QuadtreeID::clean() {
	location.x = cutBackBits(location.x, 64 - level);
	location.y = cutBackBits(location.y, 64 - level);
}

QuadtreeID QuadtreeID::parent() const {
	QuadtreeID p = *this;
	p.level--;
	return p;
}
