#pragma once

#include "Basics.hpp"

void verletPhysics(Vec3& position, Vec3& velocity, const Vec3& acceleration, Float delta);
void verletPhysics(Vec2& position, Vec2& velocity, const Vec2& acceleration, Float delta);
