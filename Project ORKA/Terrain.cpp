#include "Terrain.hpp"

void Terrain::generate(ULLVec3 location, UShort level) {
	
	//static PerlinNoise noise;
	//noise.reseed(12345);
	//long double noiseSize = 1;
	//long double x = (location.x << 64 - level) / (long double(ULLONG_MAX) / noiseSize);
	//long double y = (location.y << 64 - level) / (long double(ULLONG_MAX) / noiseSize);
	//unsigned long long target = unsigned long long(long double(noise.octaveNoise0_1(x, y, 8)) * long double(LLONG_MAX) / 8);

	hasTerrain = location.z == 0;
}