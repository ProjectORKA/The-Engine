#pragma once

#include "Basics.hpp"

struct MelonProjectile final
{
	Vec3  position = Vec3(0);
	Vec3  velocity = Vec3(0);
	Float lifeSecs = 0.0f;
	Float damage   = 0.0f;

	void update(Float dt);
	Bool isExpired(const Vec3& playerPosition, Float maxDistanceFromPlayer) const;
	Bool hitsPlayer(const Vec3& playerPosition, Float playerCollisionRadius) const;
};
