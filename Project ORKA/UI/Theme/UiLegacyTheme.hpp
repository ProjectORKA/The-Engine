#pragma once

#include "../../Basics.hpp"
#include "UiAppTheme.hpp"

struct UIElement;

namespace Ui
{
	// Maps the modern UiAppTheme palette onto legacy UIElement colors.
	struct UiLegacyTheme
	{
		[[nodiscard]] static Color panelBackground(const UiThemeMode mode);
		[[nodiscard]] static Color panelBorder(const UiThemeMode mode);
		[[nodiscard]] static Color panelHover(const UiThemeMode mode);
		[[nodiscard]] static Color panelActive(const UiThemeMode mode);
		[[nodiscard]] static Color canvasBackground(const UiThemeMode mode);

		static void applyPanelStyle(UIElement& element, const UiThemeMode mode = UiThemeMode::Dark);
		static void applyControlStyle(UIElement& element, const UiThemeMode mode = UiThemeMode::Dark);
	};
}
