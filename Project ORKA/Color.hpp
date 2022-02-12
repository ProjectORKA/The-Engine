
#include "VectorMath.hpp"
#include "Map.hpp"
#include "Name.hpp"

using Color = glm::vec4;

struct ColorPalette {
	Map<Name, Color> colorMap;

	ColorPalette() {
		colorMap["red"] = Vec4(1, 0, 0, 1);
		colorMap["green"] = Vec4(0, 1, 0, 1);
		colorMap["blue"] = Vec4(0, 0, 1, 1);
	}
};

extern ColorPalette colors;