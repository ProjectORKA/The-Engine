
#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

struct Area : public IVec2 {
	Area() {};
	Area(Int a);
	Area(Int x, Int y);
	Area center();
	void clamp(Area minimumSize);
	void clamp(Int a);
};

void logDebug(Area t);

struct TiledRectangle {
	IVec2 position = IVec2(0);
	Area size = Area(1);


	TiledRectangle() {};
	TiledRectangle(Area area) {
		this->size = area;
		this->position = IVec2(0);
	}
	IVec2 center();
	Bool positionInsideArea(IVec2 position);
};