#pragma once

#include "Basics.hpp"

void verletPhysics(Vec3& location, Vec3& velocity, const Vec3& acceleration, Float delta);
void verletPhysics(Vec2& location, Vec2& velocity, const Vec2& acceleration, Float delta);
