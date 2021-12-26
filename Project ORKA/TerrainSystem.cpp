
#include "TerrainSystem.hpp"
#include "QuadtreeSystem.hpp"
#include "PerlinNoise.hpp"

Double terrainGenerationFunction(LDouble x, LDouble y) {
	static PerlinNoise noise(TERRAIN_GENERATION_SEED);
	LDouble noiseSize = pow(2, 3);

	return pow(noise.octaveNoise0_1(noiseSize * x, noiseSize * y, 32), 3) * LDouble(pow(2,60)) / noiseSize;
};

Terrain::Terrain(QuadtreeID id)
{
	ULLVec2 location = id.location;
	LDouble xLocation = 0;
	LDouble yLocation = 0;

	lowerLimit = ULLONG_MAX;

	for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {
		for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {

			xLocation = LDouble(x - 1) / LDouble(HEIGHTMAP_FOR_NORMALS_SIZE - 3);
			yLocation = LDouble(y - 1) / LDouble(HEIGHTMAP_FOR_NORMALS_SIZE - 3);

			xLocation += LDouble(location.x) / pow(2, 64 - id.level);
			yLocation += LDouble(location.y) / pow(2, 64 - id.level);

			xLocation /= pow(2, id.level);
			yLocation /= pow(2, id.level);

			LDouble height = SEALEVEL + terrainGenerationFunction(xLocation, yLocation);

			if (height > upperLimit) upperLimit = ceil(height);
			if (height < lowerLimit) lowerLimit = floor(height);

			heightmapForNormals[x][y] = height;
		}
	}

	for (UInt y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {
		for (UInt x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
			heightmapForNormals[x][y] -= Double(lowerLimit);
			heightmapForNormals[x][y] /= pow(2, 64 - id.level);
		}
	}
}
