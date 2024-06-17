#pragma once

#include "Basics.hpp"

struct QuadtreeId
{
	UllVec2 position = UllVec2(0);
	UShort  level    = 0;

	void       clean();
	Bool       getX() const;
	Bool       getY() const;
	QuadtreeId parent() const;
};
