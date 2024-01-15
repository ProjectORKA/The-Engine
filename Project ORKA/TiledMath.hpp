#pragma once

#include "Basics.hpp"

struct Area : IVec2
{
	Area() = default;
	Area(Int x, Int y);
	explicit Area(Int a);

	void setMinimum(Int minimumSize);
	void setMinimum(Area minimumSize);

	[[nodiscard]] Area  center() const;
	[[nodiscard]] Float getAspectRatio() const;
	[[nodiscard]] Area  operator*(Float value) const;
};

void logDebug(Area t);

struct TiledRectangle
{
	Area  size     = Area(1);
	IVec2 position = IVec2(0);

	TiledRectangle();
	explicit TiledRectangle(Int size);
	explicit TiledRectangle(Area area);
	explicit TiledRectangle(Int x, Int y);
	explicit TiledRectangle(Int x, Int y, Int w, Int h);

	void operator=(const Area& other);

	[[nodiscard]] IVec2          center() const;
	[[nodiscard]] TiledRectangle operator*(Float value) const;
	[[nodiscard]] Bool           positionInsideArea(IVec2 position) const;
};
