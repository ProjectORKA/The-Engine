
#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Colliders.hpp"

struct HitResult2D {
	Bool isColliding = false;
	Float penetrationDistance = 0;
	Vec2 depenetrationVector = Vec2(0);
};

HitResult2D sphereToInfiniteWallCollision(CircleCollider c, InfiniteWallCollider2D w);