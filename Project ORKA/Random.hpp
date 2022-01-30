
#pragma once
#include "Basics.hpp"
#include "Time.hpp"
#include "Debug.hpp"

Vec2 randomVec2();
Vec3 randomVec3();
Float randomFloat();
void randomizeSeed();
ULL random(ULL size);
Vec3 randomUnitVec3();
Int randomInt(Int high);
Vec3 randomVec3(Vec3 high);
Vec2 randomVec2(Float high);
Vec3 randomVec3(Float high);
Float randomFloat(Float high);
Vec2 randomVec2(Float low, Float high);
Vec3 randomPointInSphere(Float radius);
Vec3 randomPointOnSphere(Float radius);
Vec3 randomVec3(Float low, Float high);
Float randomFloat(Float low, Float high);
Vec3 randomPointOnSphereAtLocation(Vec4 sphere);
Vec3 randomPointOnSphereAtLocation(Float radius, Vec3 location);