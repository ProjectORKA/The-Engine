#pragma once

#include "../Core/UiDocument.hpp"
#include "../Input/UiEventRouter.hpp"
#include "../Layout/UiLayoutEngine.hpp"

namespace Ui
{
	// Shift scroll content by applying a top margin on the anchor node.
	void applyScrollShift(
		UiLayoutEngine& layout, const UiNodeHandle contentNode, const Float scrollOffsetY);

	// Prevent labels/icons inside interactive rows from stealing hit tests.
	void setHitTestInvisibleRecursive(
		UiDocument& document, UiEventRouter& router, const UiNodeHandle root,
		const UiNodeHandle except = {});

	[[nodiscard]] Float listContentHeight(
		const SizeT itemCount, const Float itemHeight, const Float gap);

	[[nodiscard]] Vec2 nodeGlobalOrigin(const UiDocument& document, UiNodeHandle handle);
}
