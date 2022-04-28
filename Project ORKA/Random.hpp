
#pragma once
#include "Basics.hpp"
#include "Time.hpp"
#include "Debug.hpp"

ULong xorshf96();
Vec2 randomVec2();
Vec3 randomVec3();
Float randomFloat();
void randomizeSeed();
ULL random(ULL size);
Vec3 randomUnitVec3();
Vec2 randomUnitVec2();
Int randomInt(Int high);
Vec3 randomVec3(Vec3 high);
Vec2 randomVec2(Float high);
Vec3 randomVec3(Float high);
ULL random(ULL min, ULL max);
Float randomFloat(Float high);
Vec3 randomVec3(Vec3 low, Vec3 high);
Vec2 randomVec2(Float low, Float high);
Vec3 randomPointInSphere(Float radius);
Vec2 randomPointInCircle(Float radius);
Vec3 randomPointOnSphere(Float radius);
Vec3 randomVec3(Float low, Float high);
Float randomFloat(Float low, Float high);
Vec3 randomPointOnSphereAtLocation(Vec4 sphere);
Vec3 randomPointOnSphereAtLocation(Float radius, Vec3 location);