
#include "TerrainSystem.hpp"
#include "QuadtreeSystem.hpp"
#include "PerlinNoise.hpp"
#include "Random.hpp"

void terrainDistributionFunction(LDouble& height) {
	height = 4 * pow((height - 0.5), 3) + 0.5;
}
Terrain::Terrain(TerrainSystem& terrainSystem, QuadtreeID id, Terrain* parentTerrain, Bool cx, Bool cy)
{
	heightmap.upperLimit = 0;
	heightmap.lowerLimit = LDouble(ULLONG_MAX);

	for (UInt x = 0; x < TERRAIN_MAP_SIZE; x++) {
		for (UInt y = 0; y < TERRAIN_MAP_SIZE; y++) {
			if (parentTerrain) {
				UInt xl, xh, yl, yh;
				xl = x / 2 + TERRAIN_MAP_SIZE / 2 * cx;
				xh = xl;
				if (isOdd(x))xh++;
				yl = y / 2 + TERRAIN_MAP_SIZE / 2 * cy;
				yh = yl;
				if (isOdd(y))yh++;

				LDouble ah = parentTerrain->heightmap.height[xl][yl];
				LDouble bh = parentTerrain->heightmap.height[xh][yl];
				LDouble ch = parentTerrain->heightmap.height[xl][yh];
				LDouble dh = parentTerrain->heightmap.height[xh][yh];

				heightmap.height[x][y] = (ah + bh + ch + dh) / 4.0;
			}
			else {
				heightmap.height[x][y] = ULLONG_MAX / 2;
			}
		}
	}
	for (UInt x = 0; x < TERRAIN_MAP_SIZE; x++) {
		for (UInt y = 0; y < TERRAIN_MAP_SIZE; y++) {

			UInt sampleX = x;
			UInt sampleY = y;
			if (sampleX == TERRAIN_TEXTURE_SIZE) sampleX = 0;
			if (sampleY == TERRAIN_TEXTURE_SIZE) sampleY = 0;

			LDouble height = terrainSystem.heightTextures[id.level].data[sampleX][sampleY];
			terrainDistributionFunction(height);
			height = (height - LDouble(0.5)) * LDouble(ULLONG_MAX);
			LDouble impact = (1 / pow(2.5, id.level + 9));

			heightmap.height[x][y] += height * impact;

			if (heightmap.height[x][y] > heightmap.upperLimit) heightmap.upperLimit = heightmap.height[x][y];
			if (heightmap.height[x][y] < heightmap.lowerLimit) heightmap.lowerLimit = heightmap.height[x][y];
		}
	}
	heightmap.loaded = true;
}
TerrainSystem::TerrainSystem() {

	for (Int i = 0; i < MAX_CHUNK_LEVEL; i++) {
		CPUTexture h;

		if(i == 0) h.load("blah");
		else h.load("terrainNoise");

		for (UInt x = 0; x < TERRAIN_TEXTURE_SIZE; x++) {
			for (UInt y = 0; y < TERRAIN_TEXTURE_SIZE; y++) {
				Float xCoord = (Float(x) / Float(TERRAIN_TEXTURE_SIZE));
				Float yCoord = (Float(y) / Float(TERRAIN_TEXTURE_SIZE));

				heightTextures[i].data[x][y] = h.getRed(xCoord, yCoord);
			}
		}

		for (Int x = 0; x < TERRAIN_TEXTURE_SIZE; x++) {
			for (Int y = 0; y < TERRAIN_TEXTURE_SIZE; y++) {
				Float v, a, b, c, d;

				v = heightTextures[i].data[x][y];
				if (x < TERRAIN_TEXTURE_SIZE - 1) a = heightTextures[i].data[x + 1][y]; else a = heightTextures[i].data[0][y];
				if (y < TERRAIN_TEXTURE_SIZE - 1) b = heightTextures[i].data[x][y + 1]; else b = heightTextures[i].data[x][0];
				if (x > 0) c = heightTextures[i].data[x - 1][y]; else c = heightTextures[i].data[TERRAIN_TEXTURE_SIZE - 1][y];
				if (y > 0) d = heightTextures[i].data[x][y - 1]; else d = heightTextures[i].data[x][TERRAIN_TEXTURE_SIZE - 1];

				Float da = a - v;
				Float db = b - v;
				Float dc = c - v;
				Float dd = d - v;

				Float delta = 1.0f / Float(TERRAIN_TEXTURE_SIZE);

				Vec3 va = normalize(Vec3(delta, 0, da));
				Vec3 vb = normalize(Vec3(0, delta, db));
				Vec3 vc = normalize(Vec3(-delta, 0, dc));
				Vec3 vd = normalize(Vec3(0, -delta, dd));

				Vec3 n1 = cross(va, vb);
				Vec3 n2 = cross(vc, vd);
			}
		}
	}

	logDebug("Created TerrainSystem!");
}