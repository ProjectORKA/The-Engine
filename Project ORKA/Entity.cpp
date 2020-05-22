
#include "Entity.hpp"
#include "Renderer.hpp"

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

void SpaceShip::render(Renderer& renderer)
{
	rotation = getRotationBetweenVectors(Vec3(0, 0, 1), velocity);
	renderer.renderObjectSystem.shaderSystem.uniforms.matrices["mMatrix"] = glm::translate(Matrix(1), location) * glm::toMat4(rotation) * glm::scale(Matrix(1), Vec3(2));

	//renderer.renderObjectSystem.render("spaceShip");
	renderer.renderObjectSystem.render("spaceShipLOD");
}
