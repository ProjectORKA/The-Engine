#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f

float randomFloat(Float low, Float high);

float clamp(Float a, Float min, Float max);

bool isFloatNearOther(Float a, Float b, Float error);