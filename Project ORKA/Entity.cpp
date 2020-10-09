
#include "Entity.hpp"

void SpaceShip::update(Time& time)
{
	if (!time.paused) {

		Vec3 tangent = normalize(glm::cross(velocity, location));
		Vec3 orbitTarget = normalize(glm::cross(location, tangent));
		Vec3 altitudeTarget = normalize(location) * Vec3(700) - location;

		location += velocity * time.delta;
		velocity += orbitTarget + altitudeTarget * Vec3(0.01);
		velocity *= 0.95;
	}
}
