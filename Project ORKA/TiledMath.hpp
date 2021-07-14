
#pragma once

#include "Basics.hpp"

using Area = IVec2;

struct TiledRectangle {
	IVec2 position = IVec2(0);
	Area size = Area(1);

	IVec2 center();
	Bool positionInsideArea(IVec2 position);
};

Area center(Area area);