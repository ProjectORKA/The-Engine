#include "Physics.hpp"

void verletPhysics(Vec3& location, Vec3& velocity, const Vec3& acceleration, const Float delta) {
	//step 1
	const Vec3 velocity1 = velocity + acceleration * delta / Vec3(2);

	//step 2
	location += velocity1 * delta;

	//step 3 calculate forces

	//step4
	const Vec3 velocity2 = velocity1 + acceleration * delta / Vec3(2);

	velocity = velocity2;
}

void verletPhysics(Vec2& location, Vec2& velocity, const Vec2& acceleration, const Float delta) {
	//step 1
	const Vec2 velocity1 = velocity + acceleration * delta / Vec2(2);

	//step 2
	location += velocity1 * delta;

	//step 3 calculate forces

	//step4
	const Vec2 velocity2 = velocity1 + acceleration * delta / Vec2(2);

	velocity = velocity2;
}

void collisionBetween2DSphereAndWall(Vec2& velocity, const Vec2 wallDirection, Vec2 locationOfSphereOnCoantact) {
	velocity = reflect(velocity, wallDirection);
}
