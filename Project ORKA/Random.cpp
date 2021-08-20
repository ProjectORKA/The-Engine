
#include "Random.hpp"

ULL random(ULL size) {
	std::random_device seed;
	std::mt19937_64 randomizer(seed());
	std::uniform_int_distribution<unsigned long long> distribution;

	return distribution(randomizer) / (ULLONG_MAX / size);
}

float randomFloat()
{
	return randomFloat(0, 1);
}
float randomFloat(Float low, Float high)
{
	return low + static_cast <Float> (rand()) / (static_cast <Float> (RAND_MAX / (high - low)));
}