#include "MelonProjectile.hpp"

void MelonProjectile::update(Float dt)
{
	position += velocity * dt;
	lifeSecs -= dt;
}

Bool MelonProjectile::isExpired(const Vec3& playerPosition, Float maxDistanceFromPlayer) const
{
	return lifeSecs <= 0.0f || distance(position, playerPosition) > maxDistanceFromPlayer;
}

Bool MelonProjectile::hitsPlayer(const Vec3& playerPosition, Float playerCollisionRadius) const
{
	return distance(position, playerPosition) <= playerCollisionRadius;
}
