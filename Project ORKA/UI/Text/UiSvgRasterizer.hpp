#pragma once

#include "UiSvgTypes.hpp"

namespace Ui
{
	class UiSvgRasterizer
	{
	public:
		[[nodiscard]] static Bool loadDocument(const Path& path, UiSvgDocument& output);
		[[nodiscard]] static Bool rasterize(
			const UiSvgDocument& document, UInt pixelSize, UiSvgRasterResult& output);
	};
} // namespace Ui
