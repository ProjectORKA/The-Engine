#include "TerrainSystem.hpp"

PerlinNoise noise;

#define TERRAIN_GENERATION_SEED 4797655348219768357

void Terrain::create()
{
	noise.reseed(TERRAIN_GENERATION_SEED);

	for (int x = 0; x < TERRAIN_DETAIL+1; x++) {
		for (int y = 0; y < TERRAIN_DETAIL+1; y++) {
			heightmap[x][y] = randomFloat();
		}
	}
}