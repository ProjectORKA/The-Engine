#include "QuadtreeId.hpp"

Bool QuadtreeId::getX() const {
	if (level == 0) return false;
	return BitSet<64>(location.x)[64 - level];
}

Bool QuadtreeId::getY() const {
	if (level == 0) return false;
	return BitSet<64>(location.y)[64 - level];
}

void QuadtreeId::clean() {
	location.x = cutBackBits(location.x, 64 - level);
	location.y = cutBackBits(location.y, 64 - level);
}

QuadtreeId QuadtreeId::parent() const {
	QuadtreeId p = *this;
	p.level--;
	return p;
}
