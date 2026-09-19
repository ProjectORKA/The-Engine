#pragma once

#include "../Core/UiDocument.hpp"
#include "../Layout/UiLayoutEngine.hpp"
#include "../Render/UiDisplayList.hpp"

namespace Ui::Inspector
{
	struct UiLayoutDebugOverlayStyle
	{
		UiLinearColor contentColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.435f, 0.659f, 0.863f, 0.55f));
		UiLinearColor paddingColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.576f, 0.769f, 0.490f, 0.45f));
		UiLinearColor marginColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.965f, 0.698f, 0.420f, 0.50f));
		UiLinearColor gapColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.608f, 0.482f, 0.722f, 0.45f));
		UiLinearColor borderOutlineColor =
			UiLinearColor::fromStraightSrgb(Vec4(0.15f, 0.15f, 0.15f, 0.85f));
		Float outlineWidth = 1.0f;
	};

	struct UiLayoutDebugOverlayResult
	{
		Bool valid = true;
		Bool painted = false;
	};

	class UiLayoutDebugOverlay
	{
	public:
		[[nodiscard]] static UiLayoutDebugOverlayResult paintHovered(
			const UiDocument& document,
			const UiLayoutEngine& layoutEngine,
			UiNodeHandle hovered,
			UiDisplayList& output,
			const UiLayoutDebugOverlayStyle& style = {});
	};
}
