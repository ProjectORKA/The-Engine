#pragma once

#include <random>
#include "Basics.hpp"

extern std::random_device seed;

ULong xorshf96();
Vec2  randomVec2();
Vec3  randomVec3();
Float randomFloat();
void  randomizeSeed();
ULL   random(ULL size);
Vec3  randomUnitVec3();
Vec2  randomUnitVec2();
Int   randomInt(Int max);
UInt  randomUInt(UInt max);
Vec3  randomVec3(Vec3 max);
Vec2  randomVec2(Float max);
Vec3  randomVec3(Float max);
ULL   random(ULL min, ULL max);
Float randomFloat(Float max);
Vec3  randomVec3(Vec3 min, Vec3 max);
Vec4  randomVec4(Float min, Float max);
Vec2  randomVec2(Float min, Float max);
Vec3  randomPointInSphere(Float radius);
Vec2  randomPointInCircle(Float radius);
Vec3  randomPointOnSphere(Float radius);
Vec3  randomVec3(Float min, Float max);
Float randomFloat(Float min, Float max);
Vec3  randomPointOnSphereAtLocation(Vec4 sphere);
Vec3  randomPointOnSphereAtLocation(Float radius, Vec3 location);
