#pragma once

#include "UiPaintBuilder.hpp"
#include "../Text/UiTextPainter.hpp"

namespace Ui
{
	class UiSvgIconService;

	// Theme-neutral fallback used by engine samples and compatibility hosts.
	// A provider can substitute resident grayscale/MSDF/color atlas handles;
	// missing glyphs continue through the renderer-owned built-in font.
	[[nodiscard]] UiNodePainter makeBasicThemePainter(
		UiTextGlyphProvider glyphProvider = {},
		UiTextStyleProvider textStyleProvider = {},
		UiTextShapeProvider textShapeProvider = {},
		UiSvgIconService* iconService = nullptr);
}
