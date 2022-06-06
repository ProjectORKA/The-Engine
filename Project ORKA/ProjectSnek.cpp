#include "ProjectSnek.hpp"

void Snake::update(SnakeFood& snakefood) {
	headPosition = headPosition + direction;
	if (length(snakefood.foodPosition - headPosition) < 0.06) {
		snakefood.foodPosition = randomVec2(-0.9, 0.9);
		addSegment();
	}
	for (Int i = 0; i < bodySegments.size(); i++) {
		if (i == 0) {
			bodySegments[i] = normalize(bodySegments[i] - headPosition) * Vec2(0.03, 0.03) + headPosition;
		}
		else{
			bodySegments[i] = normalize(bodySegments[i] - bodySegments[i-1]) * Vec2(0.03, 0.03) + bodySegments[i-1];
		}
	}
}
