#pragma once

#include "Basics.hpp"
#include "Debug.hpp"

struct Area : public IVec2 {
	Area() {};
	Area(Int a);
	Area center() const;
	Area(Int x, Int y);
	Area operator*(Float value) const;
	void setMinimum(Int minimumSize);
	void setMinimum(Area minimumSize);
};

void logDebug(Area t);

struct TiledRectangle {
	Area size = Area(1);
	IVec2 position = IVec2(0);

	IVec2 center() const;
	TiledRectangle() {};
	TiledRectangle(Area area);
	TiledRectangle operator*(Float value) const;
	Bool positionInsideArea(IVec2 position) const;
	TiledRectangle(Int x, Int y, Int w, Int h);
};
