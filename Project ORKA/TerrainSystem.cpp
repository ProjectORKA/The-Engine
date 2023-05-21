#include "TerrainSystem.hpp"
#include "QuadTreeSystem.hpp"
#include "PerlinNoise.hpp"
#include "Random.hpp"

void TerrainSystem::create(ResourceManager& resourceManager) {
	for (Int i = 0; i < MAX_CHUNK_LEVEL; i++) {
		CPUTexture h;

		if (i == 0) h.load(resourceManager, "blah", Filter::linear, Filter::linear, Wrapping::repeat);
		else h.load(resourceManager, "terrainNoise", Filter::linear, Filter::linear, Wrapping::repeat);

		for (UInt x = 0; x < TERRAIN_TEXTURE_SIZE; x++) {
			for (UInt y = 0; y < TERRAIN_TEXTURE_SIZE; y++) {
				Float xCoord = (static_cast<Float>(x) / static_cast<Float>(TERRAIN_TEXTURE_SIZE));
				Float yCoord = (static_cast<Float>(y) / static_cast<Float>(TERRAIN_TEXTURE_SIZE));

				heightTextures[i].data[x][y] = h.getRed(xCoord, yCoord);
			}
		}

		for (Int x = 0; x < TERRAIN_TEXTURE_SIZE; x++) {
			for (Int y = 0; y < TERRAIN_TEXTURE_SIZE; y++) {
				Float v, a, b, c, d;

				v = heightTextures[i].data[x][y];
				if (x < TERRAIN_TEXTURE_SIZE - 1) a = heightTextures[i].data[x + 1][y];
				else a = heightTextures[i].data[0][y];
				if (y < TERRAIN_TEXTURE_SIZE - 1) b = heightTextures[i].data[x][y + 1];
				else b = heightTextures[i].data[x][0];
				if (x > 0) c = heightTextures[i].data[x - 1][y];
				else c = heightTextures[i].data[TERRAIN_TEXTURE_SIZE - 1][y];
				if (y > 0) d = heightTextures[i].data[x][y - 1];
				else d = heightTextures[i].data[x][TERRAIN_TEXTURE_SIZE - 1];

				Float da = a - v;
				Float db = b - v;
				Float dc = c - v;
				Float dd = d - v;

				Float delta = 1.0f / static_cast<Float>(TERRAIN_TEXTURE_SIZE);

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

void terrainDistributionFunction(LDouble& height) { height = 4 * pow((height - 0.5), 3) + 0.5; }

Terrain::Terrain(const TerrainSystem& terrainSystem, const QuadtreeId& id, const Terrain* parentTerrain, const Bool cx,
                 const Bool cy) {
	heightmap.upperLimit = 0;
	heightmap.lowerLimit = static_cast<LDouble>(ULLONG_MAX);

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

				const LDouble ah = parentTerrain->heightmap.height[xl][yl];
				const LDouble bh = parentTerrain->heightmap.height[xh][yl];
				const LDouble ch = parentTerrain->heightmap.height[xl][yh];
				const LDouble dh = parentTerrain->heightmap.height[xh][yh];

				heightmap.height[x][y] = (ah + bh + ch + dh) / 4.0;
			}
			else { heightmap.height[x][y] = ULLONG_MAX / 2; }
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
			height = (height - static_cast<LDouble>(0.5)) * static_cast<LDouble>(ULLONG_MAX);
			const LDouble impact = (3 / pow(2.5, id.level + 7));

			heightmap.height[x][y] += height * impact;

			if (heightmap.height[x][y] > heightmap.upperLimit) heightmap.upperLimit = heightmap.height[x][y];
			if (heightmap.height[x][y] < heightmap.lowerLimit) heightmap.lowerLimit = heightmap.height[x][y];
		}
	}
	heightmap.loaded = true;
}
