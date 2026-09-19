#include "UiLegacyTheme.hpp"

#include "../../UIElement.hpp"

namespace Ui
{
	namespace
	{
		[[nodiscard]] Float linearToSrgb(const Float channel)
		{
			if (channel <= 0.0031308f) return 12.92f * channel;
			return 1.055f * std::pow(channel, 1.0f / 2.4f) - 0.055f;
		}

		[[nodiscard]] Color fromLinear(const UiLinearColor& color)
		{
			const Vec4 premul = color.premultiplied;
			const Float alpha = premul.w > 0.0f ? premul.w : 1.0f;
			const Vec3 straightLinear(premul.x / alpha, premul.y / alpha, premul.z / alpha);
			return Color(
				linearToSrgb(straightLinear.x),
				linearToSrgb(straightLinear.y),
				linearToSrgb(straightLinear.z),
				alpha);
		}

		[[nodiscard]] const UiAppColors& colorsFor(const UiThemeMode mode)
		{
			static const UiAppColors light = UiAppTheme::light().colors;
			static const UiAppColors dark = UiAppTheme::dark().colors;
			return mode == UiThemeMode::Light ? light : dark;
		}
	}

	Color UiLegacyTheme::panelBackground(const UiThemeMode mode)
	{
		return fromLinear(colorsFor(mode).surfacePanel);
	}

	Color UiLegacyTheme::panelBorder(const UiThemeMode mode)
	{
		return fromLinear(colorsFor(mode).border);
	}

	Color UiLegacyTheme::panelHover(const UiThemeMode mode)
	{
		return fromLinear(colorsFor(mode).surfaceControlHover);
	}

	Color UiLegacyTheme::panelActive(const UiThemeMode mode)
	{
		return fromLinear(colorsFor(mode).surfaceControlActive);
	}

	Color UiLegacyTheme::canvasBackground(const UiThemeMode mode)
	{
		return fromLinear(colorsFor(mode).surfaceRoot);
	}

	void UiLegacyTheme::applyPanelStyle(UIElement& element, const UiThemeMode mode)
	{
		element.background(panelBackground(mode));
		element.hoveredBackground(panelBackground(mode));
		element.activeBackground(panelBackground(mode));
		element.border(1, panelBorder(mode));
	}

	void UiLegacyTheme::applyControlStyle(UIElement& element, const UiThemeMode mode)
	{
		element.background(fromLinear(colorsFor(mode).surfaceControl));
		element.hoveredBackground(fromLinear(colorsFor(mode).surfaceControlHover));
		element.activeBackground(fromLinear(colorsFor(mode).surfaceControlActive));
		element.border(1, panelBorder(mode));
	}
}
