
#include "Random.hpp"

ULL random(ULL size) {
	std::random_device seed;
	std::mt19937_64 randomizer(seed());
	std::uniform_int_distribution<unsigned long long> distribution;

	return distribution(randomizer) / (ULLONG_MAX / size);
}