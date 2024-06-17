#include "QuadtreeId.hpp"
#include "ULLUtil.hpp"

void QuadtreeId::clean()
{
	position.x = cutBackBits(position.x, 64 - level);
	position.y = cutBackBits(position.y, 64 - level);
}

Bool QuadtreeId::getX() const
{
	if(level == 0) return false;
	return BitSet<64>(position.x)[64 - level];
}

Bool QuadtreeId::getY() const
{
	if(level == 0) return false;
	return BitSet<64>(position.y)[64 - level];
}

QuadtreeId QuadtreeId::parent() const
{
	QuadtreeId p = *this;
	p.level--;
	return p;
}
