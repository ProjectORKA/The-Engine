#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f


float max(Float a, Float b);
float min(Float a, Float b);
float randomFloat();
float randomFloat(Float low, Float high);
float clamp(Float a, Float min, Float max);
bool isFloatNearOther(Float a, Float b, Float error);
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);
