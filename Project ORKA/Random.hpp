
#pragma once

#include "Basics.hpp"
#include <iostream>
#include <random>
#include <limits>

void randomizeSeed();

Int randomInt(Int high);

ULL random(ULL size);

Float randomFloat();
Float randomFloat(Float high);
Float randomFloat(Float low, Float high);

Vec3 randomVec3();
Vec3 randomUnitVec3();
Vec3 randomVec3(Vec3 high);
Vec3 randomPointInSphere(Float radius);
Vec3 randomVec3(Float low, Float high);