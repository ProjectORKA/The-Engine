
#pragma once

#include "Basics.hpp"

struct UnitCircleCollider {
	Vec2 position = Vec2(0);
};

struct CircleCollider {
	Float radius = 1;
	Vec2 position = Vec2(0);
};

struct InfiniteWallCollider2D {
	Vec2 position = Vec2(0);
	Vec2 direction = Vec2(0, 1);
};