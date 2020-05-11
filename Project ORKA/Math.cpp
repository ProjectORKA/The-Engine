
#include "Math.hpp"

float randomFloat(Float low, Float high)
{
	return low + static_cast <Float> (rand()) / (static_cast <Float> (RAND_MAX / (high - low)));
}

float clamp(Float a, Float min, Float max) {
	if (a >= max) return max;
	if (a <= min) return min;
	return a;
};

bool isFloatNearOther(Float a, Float b, Float error) {
	return fabsf(a - b) < error;
};