#pragma once

#include "Basics.hpp"

struct UnitCircleCollider
{
	Vec2 position = Vec2(0);
};

struct CircleCollider
{
	Float radius   = 1;
	Vec2  position = Vec2(0);
};

struct InfiniteWallCollider2D
{
	Vec2 position  = Vec2(0);
	Vec2 direction = Vec2(0, 1);
};

struct AABB
{
	Float n = 0;
	Float s = 0;
	Float w = 0;
	Float e = 0;
};

struct HitResult2D
{
	Bool  isColliding         = false;
	Float penetrationDistance = 0;
	Vec2  depenetrationVector = Vec2(0);
};

Bool        aabbCollision(AABB a, AABB b);
HitResult2D sphereToInfiniteWallCollision(CircleCollider c, InfiniteWallCollider2D w);
