#include "UiBasicTheme.hpp"

#include "../Theme/UiAppTheme.hpp"

namespace Ui
{
	UiNodePainter makeBasicThemePainter(
		UiTextGlyphProvider glyphProvider,
		UiTextStyleProvider textStyleProvider,
		UiTextShapeProvider textShapeProvider,
		UiSvgIconService* iconService)
	{
		return makeAppThemePainter(
			UiAppTheme::light(), nullptr, std::move(textStyleProvider),
			std::move(glyphProvider), std::move(textShapeProvider), iconService);
	}
}
