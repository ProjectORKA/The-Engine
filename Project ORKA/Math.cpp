#include "Math.hpp"
#include <random>


float randomFloat(float low, float high)
{
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}
