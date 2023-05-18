#pragma once

#include "Math.hpp"
#include "TiledMath.hpp"
#include "Basics.hpp"
#include "GraphicsAPI.hpp"

struct RenderRegion {
	TiledRectangle region;

	void set(Area area);
	void set(TiledRectangle region);
	Float getAspectRatio() const;
};
