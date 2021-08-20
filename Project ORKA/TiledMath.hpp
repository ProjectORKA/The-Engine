
#pragma once

#include "Basics.hpp"

struct Area : public IVec2 {
	Area() {};
	Area(Int a);
	Area(Int x, Int y);
	Area center();
	void clamp(Area minimumSize);
	void clamp(Int a);;
};

struct TiledRectangle {
	IVec2 position = IVec2(0);
	Area size = Area(1);

	IVec2 center();
	Bool positionInsideArea(IVec2 position);
};