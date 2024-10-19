#pragma once

#include "TiledMath.hpp"
#include "Basics.hpp"

struct Renderer;

//[TODO] check if this can be removed / simplified
struct RenderRegion
{
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] Float getAspectRatio() const;

	RenderRegion();
	explicit RenderRegion(Area area);
	void     set(Renderer& renderer, const IVec2& area);
	void     set(Renderer& renderer, const TiledRectangle& newRegion);

private:
	IVec2 size;
	IVec2 position;
};
