
#include "TerrainSystem.hpp"

Double terrainGenerationFunction(Double x, Double y) {
	static PerlinNoise noise(TERRAIN_GENERATION_SEED);
	Double height = SEA_LEVEL;
	height += pow(noise.octaveNoise0_1(6 * x, 6 * y, 16), 2) * LDouble(LLONG_MAX / 128);
	return height;
};

void Terrain::create(QuadtreeID id)
{
	ULLVec2 location = id.location;
	LDouble xLocation = 0;
	LDouble yLocation = 0;

	lowerLimit = ULLONG_MAX;

	for (Int x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
		for (Int y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {

			xLocation = LDouble(x - 1) / LDouble(HEIGHTMAP_FOR_NORMALS_SIZE - 3);
			yLocation = LDouble(y - 1) / LDouble(HEIGHTMAP_FOR_NORMALS_SIZE - 3);

			xLocation += LDouble(location.x) / pow(2, 64 - id.level);
			yLocation += LDouble(location.y) / pow(2, 64 - id.level);

			xLocation /= pow(2, id.level - 3);
			yLocation /= pow(2, id.level - 3);

			LDouble height = terrainGenerationFunction(xLocation, yLocation);

			if (height > upperLimit) upperLimit = ceil(height);
			if (height < lowerLimit) lowerLimit = floor(height);

			heightmapForNormals[y][x] = height;
		}
	}

	for (UInt x = 0; x < HEIGHTMAP_FOR_NORMALS_SIZE; x++) {
		for (UInt y = 0; y < HEIGHTMAP_FOR_NORMALS_SIZE; y++) {
			heightmapForNormals[y][x] -= Double(lowerLimit);
			heightmapForNormals[y][x] /= pow(2, 64 - id.level);
		}
	}
}