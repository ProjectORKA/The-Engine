#include "HeightMap.hpp"

//CPUTexture generateHeightMap() {
//
//	static PerlinNoise noise;
//	noise.reseed(12345);
//
//	CPUTexture heightmap;
//	heightmap.channels = 1;
//	heightmap.dataType = dataTypeFloat;
//	heightmap.filter = linear;
//	heightmap.height = 1024;
//	heightmap.width = 1024;
//	heightmap.name = "terrainHeightmap";
//	heightmap.wrapping = clamped;
//	heightmap.pixels = new Float[1024 * 1024];
//
//	for (int x = 0; x < 1024; x++) {
//		for (int y = 0; y < 1024; y++) {
//			heightmap.floatPixels[y * 1024 + x] = 1.0f;// noise.octaveNoise0_1(float(x) / 1024, float(y));
//		}
//	}
//
//	heightmap.loaded = true;
//
//	return heightmap;
//}
