
#pragma once

#include "Basics.hpp"
#include "Math.hpp"
#include "Colliders.hpp"

struct AABB {
	Float n = 0;
	Float s = 0;
	Float w = 0;
	Float e = 0;
};

struct HitResult2D {
	Bool isColliding = false;
	Float penetrationDistance = 0;
	Vec2 depenetrationVector = Vec2(0);
};

Bool aabbCollision(AABB a, AABB b);
HitResult2D sphereToInfiniteWallCollision(CircleCollider c, InfiniteWallCollider2D w);
