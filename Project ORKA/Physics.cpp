
#include "Physics.hpp"

void verletPhysics(Vec3& location, Vec3& velocity, Vec3& acceleration, Float delta) {

	//step 1
	Vec3 velocity1 = velocity + acceleration * delta / Vec3(2);

	//step 2
	location += velocity1 * delta;

	//step 3 calculate forces

	//step4
	Vec3 velocity2 = velocity1 + acceleration * delta / Vec3(2);

	velocity = velocity2;
}

void verletPhysics(Vec2& location, Vec2& velocity, Vec2& acceleration, Float delta) {

	//step 1
	Vec2 velocity1 = velocity + acceleration * delta / Vec2(2);

	//step 2
	location += velocity1 * delta;

	//step 3 calculate forces

	//step4
	Vec2 velocity2 = velocity1 + acceleration * delta / Vec2(2);

	velocity = velocity2;
}

void collisionBetween2DSphereAndWall(Vec2& velocity, Vec2 wallDirection, Vec2 locationOfSphereOnCoantact) {
	velocity = glm::reflect(velocity, wallDirection);
}
