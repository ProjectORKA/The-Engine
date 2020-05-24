#pragma once

#include "BasicsModule.hpp"

#define PI 3.14159265359f

Float max(Float a, Float b);
Float min(Float a, Float b);
Float lerp(Float a, Float b, Float alpha);
Float randomFloat();
Float randomFloat(Float low, Float high);
Float clamp(Float a, Float min, Float max);
bool isFloatNearOther(Float a, Float b, Float error);
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);
