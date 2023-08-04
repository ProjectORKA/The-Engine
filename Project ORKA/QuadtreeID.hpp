#pragma once

#include "Basics.hpp"

struct QuadtreeId
{
	ULLVec2 location = ULLVec2(0);
	UShort  level    = 0;

	void       clean();
	Bool       getX() const;
	Bool       getY() const;
	QuadtreeId parent() const;
};
