
#include "Math.hpp"

float max(Float a, Float b) {
	return (a < b) ? b : a;
}
float min(Float a, Float b) {
	return !(b < a) ? a : b;
}
float clamp(Float a, Float min, Float max) {
	if (a >= max) return max;
	if (a <= min) return min;
	return a;
}
float randomFloat(Float low, Float high)
{
	return low + static_cast <Float> (rand()) / (static_cast <Float> (RAND_MAX / (high - low)));
}
bool isFloatNearOther(Float a, Float b, Float error) {
	return fabsf(a - b) < error;
}