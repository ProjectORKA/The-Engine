#include "PhysicsUtil.hpp"

void loopPointInSpace2D(Vec2& point, Vec2 size) {
	if (point.x < 0) point.x = size.x;
	if (point.x > size.x) point.x = 0;
	if (point.y < 0) point.y = size.y;
	if (point.y >= size.y) point.y = 0;
}

void loopPointInCenteredSpace2D(Vec2& point, Vec2 size) {
	if (point.x < -size.x) point.x = size.x;
	if (point.x > size.x) point.x = -size.x;

	if (point.y < -size.y) point.y = size.y;
	if (point.y > size.y) point.y = -size.y;
}
