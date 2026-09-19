#pragma once

#include "../../Basics.hpp"

#include <glm/glm.hpp>

namespace Ui
{
	struct UiSvgSubpath
	{
		Vector<glm::vec2> points;
		Bool closed = false;
	};

	struct UiSvgDocument
	{
		Float viewBoxWidth = 24;
		Float viewBoxHeight = 24;
		Float strokeWidth = 2;
		Vector<UiSvgSubpath> subpaths;
	};

	struct UiSvgRasterResult
	{
		UInt width = 0;
		UInt height = 0;
		Vector<Byte> rgba;
	};

} // namespace Ui
