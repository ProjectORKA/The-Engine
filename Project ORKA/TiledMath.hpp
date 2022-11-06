
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

struct Area : public IVec2 {
	Area() {};
	Area(Int a);
	Area(Int x, Int y);
	Area center();
	void setMinimum(Area minimumSize);
	void setMinimum(Int minimumSize);
};

void logDebug(Area t);

struct TiledRectangle {
	Area size = Area(1);
	IVec2 position = IVec2(0);

	IVec2 center();
	TiledRectangle() {};
	TiledRectangle(Area area);
	Bool positionInsideArea(IVec2 position);
	TiledRectangle(Int x, Int y, Int w, Int h);
};