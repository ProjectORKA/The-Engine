#include "UiInteractiveSurface.hpp"

namespace Ui
{
	UiInteractiveSurfaceGlViewport interactiveSurfaceGlViewport(
		const UiInteractiveSurfaceContext& context, const Int windowHeightLogical)
	{
		const IVec2 size(
			static_cast<Int>(context.size.x),
			static_cast<Int>(context.size.y));
		const IVec2 position(
			static_cast<Int>(context.position.x),
			windowHeightLogical - static_cast<Int>(context.position.y + context.size.y));
		return {position, size};
	}

	DVec2 interactiveSurfacePointerLocalBottomLeft(
		const UiInteractiveSurfaceContext& context, const DVec2 pointerTopLeftLogical)
	{
		const Double localTopLeftY = pointerTopLeftLogical.y - context.position.y;
		return {
			pointerTopLeftLogical.x - context.position.x,
			context.size.y - localTopLeftY
		};
	}

	Bool interactiveSurfaceContainsPointer(
		const UiInteractiveSurfaceContext& context, const DVec2 pointerTopLeftLogical)
	{
		if (context.size.x <= 0 || context.size.y <= 0) return false;
		const DVec2 local = pointerTopLeftLogical - DVec2(context.position);
		return local.x >= 0 && local.y >= 0 &&
			local.x < context.size.x && local.y < context.size.y;
	}
}
