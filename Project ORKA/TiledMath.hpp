
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

	Area operator*(Float value) {
		return Area(x * value, y * value);
	};
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

	TiledRectangle operator*(Float value) {
		return TiledRectangle(size.x * value, size.y * value, position.x + (size.x * (1 - value) / 2), position.y + (size.y * (1 - value) / 2));
	};
};