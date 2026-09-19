#pragma once

#include "UiSvgTypes.hpp"

namespace Ui
{
	class UiSvgPathParser
	{
	public:
		[[nodiscard]] static Bool parsePathData(const String& data, Vector<UiSvgSubpath>& output);
	};
} // namespace Ui
