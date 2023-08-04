#pragma once

#include "TiledMath.hpp"
#include "Basics.hpp"

struct RenderRegion
{
	RenderRegion();
	void                set(Area area);
	[[nodiscard]] Float getAspectRatio() const;
	explicit            RenderRegion(Area area);
	void                set(const TiledRectangle& newRegion);
private:
	TiledRectangle region;
};
