#pragma once

#include "../Widgets/UiWidgets.hpp"

namespace Ui
{
	struct UiInteractiveSurfaceGlViewport
	{
		IVec2 position = IVec2(0);
		IVec2 size = IVec2(0);
	};

	[[nodiscard]] UiInteractiveSurfaceGlViewport interactiveSurfaceGlViewport(
		const UiInteractiveSurfaceContext& context, Int windowHeightLogical);

	[[nodiscard]] DVec2 interactiveSurfacePointerLocalBottomLeft(
		const UiInteractiveSurfaceContext& context, DVec2 pointerTopLeftLogical);

	[[nodiscard]] Bool interactiveSurfaceContainsPointer(
		const UiInteractiveSurfaceContext& context, DVec2 pointerTopLeftLogical);
}
