#include "TerrainSystem.hpp"
#include "CPUTexture.hpp"
#include "Math.hpp"
#include "QuadtreeSystem.hpp"

void terrainDistributionFunction(LDouble& height)
{
	height = 4 * pow(height - 0.5, 3) + 0.5;
}

void TerrainSystem::create()
{
	for (Int i = 0; i < MAX_CHUNK_LEVEL; i++)
	{
		CpuTexture h;

		if (i == 0) h.load(Name("blah"), Filter::Linear, Filter::Linear, Wrapping::Repeat);
		else h.load(Name("terrainNoise"), Filter::Linear, Filter::Linear, Wrapping::Repeat);

		for (UInt x = 0; x < TERRAIN_TEXTURE_SIZE; x++)
		{
			for (UInt y = 0; y < TERRAIN_TEXTURE_SIZE; y++)
			{
				const Float xPos = static_cast<Float>(x) / static_cast<Float>(TERRAIN_TEXTURE_SIZE);
				const Float yPos = static_cast<Float>(y) / static_cast<Float>(TERRAIN_TEXTURE_SIZE);

				heightTextures[i].setHeight(x, y, h.getRed(xPos, yPos));
			}
		}

		// for(Int x = 0; x < TERRAIN_TEXTURE_SIZE; x++)
		// {
		//	for(Int y = 0; y < TERRAIN_TEXTURE_SIZE; y++)
		//	{
		//		Float v, a, b, c, d;

		//		v = heightTextures[i].getHeight(x,y);
		//		if(x < TERRAIN_TEXTURE_SIZE - 1) a = heightTextures[i].getHeight(x + 1,y);
		//		else a                             = heightTextures[i].getHeight(0,y);
		//		if(y < TERRAIN_TEXTURE_SIZE - 1) b = heightTextures[i].getHeight(x,y + 1);
		//		else b                             = heightTextures[i].getHeight(x,0);
		//		if(x > 0) c = heightTextures[i].getHeight(x - 1,y);
		//		else c      = heightTextures[i].getHeight(TERRAIN_TEXTURE_SIZE - 1,y);
		//		if(y > 0) d = heightTextures[i].getHeight(x,y - 1);
		//		else d      = heightTextures[i].getHeight(x,TERRAIN_TEXTURE_SIZE - 1);

		//		Float da = a - v;
		//		Float db = b - v;
		//		Float dc = c - v;
		//		Float dd = d - v;

		//		Float delta = 1.0f / static_cast<Float>(TERRAIN_TEXTURE_SIZE);

		//		// Vec3 va = normalize(Vec3(delta, 0, da));
		//		// Vec3 vb = normalize(Vec3(0, delta, db));
		//		// Vec3 vc = normalize(Vec3(-delta, 0, dc));
		//		// Vec3 vd = normalize(Vec3(0, -delta, dd));

		//		// Vec3 n1 = cross(va, vb);
		//		// Vec3 n2 = cross(vc, vd);
		//	}
		// }
	}

	logDebug("Created TerrainSystem!");
}

AdvancedHeightMap& Terrain::getHeightmap()
{
	return heightmap;
}

const LDouble (& Terrain::getHeights() const)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE]
{
	return heightmap.getHeights();
}

LDouble Terrain::getLowerLimit() const
{
	return heightmap.getLowerLimit();
}

LDouble Terrain::getUpperLimit() const
{
	return heightmap.getUpperLimit();
}

Float HeightTexture::getHeight(const Int x, const Int y) const
{
	return data[x][y];
}

void HeightTexture::setHeight(const UInt x, const UInt y, const Float value)
{
	data[x][y] = value;
}

Terrain::Terrain(const TerrainSystem& terrainSystem, const QuadtreeId& id, const Terrain* parentTerrain, const Bool cx, const Bool cy)
{
	LDouble       localUpperLimit = 0;
	LDouble       localLowerLimit = static_cast<LDouble>(ULLONG_MAX);
	const LDouble impact          = 3.0 / std::pow(2.2, id.level + 6);
	LDouble       heightArray[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE];
	for (UInt x = 0; x < TERRAIN_MAP_SIZE; ++x)
	{
		const UInt xl = x / 2 + (cx * (TERRAIN_MAP_SIZE / 2));
		const UInt xh = xl + (x % 2);
		for (UInt y = 0; y < TERRAIN_MAP_SIZE; ++y)
		{
			LDouble baseHeight;
			if (parentTerrain)
			{
				const UInt    yl      = y / 2 + (cy * (TERRAIN_MAP_SIZE / 2));
				const UInt    yh      = yl + (y % 2);
				const auto&   pHeight = parentTerrain->getHeights();
				const LDouble ah      = pHeight[xl][yl];
				const LDouble bh      = pHeight[xh][yl];
				const LDouble ch      = pHeight[xl][yh];
				const LDouble dh      = pHeight[xh][yh];
				baseHeight            = (ah + bh + ch + dh) * 0.25;
			}
			else baseHeight = static_cast<LDouble>(ULLONG_MAX) * 0.5;
			const UInt sampleX   = x % TERRAIN_TEXTURE_SIZE;
			const UInt sampleY   = y % TERRAIN_TEXTURE_SIZE;
			LDouble    texHeight = terrainSystem.heightTextures[id.level].getHeight(sampleX, sampleY);
			terrainDistributionFunction(texHeight);
			texHeight                 = (texHeight - 0.5L) * static_cast<LDouble>(ULLONG_MAX);
			const LDouble finalHeight = baseHeight + texHeight * impact;
			if (finalHeight > localUpperLimit) localUpperLimit = finalHeight;
			if (finalHeight < localLowerLimit) localLowerLimit = finalHeight;
			heightArray[x][y] = finalHeight;
		}
	}
	heightmap.setHeights(heightArray);

	// Store final min/max into the heightmap once
	heightmap.setUpperLimit(localUpperLimit);
	heightmap.setLowerLimit(localLowerLimit);
	heightmap.setLoaded(true);
}

Terrain::~Terrain()
{
}
