#pragma once

#include "Basics.hpp"
#include "Collision.hpp"
#include "PhysicsUtil.hpp"

void verletPhysics(Vec3& location, Vec3& velocity, Vec3& acceleration, Float delta);
void verletPhysics(Vec2& location, Vec2& velocity, Vec2& acceleration, Float delta);