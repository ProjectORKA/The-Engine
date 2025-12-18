#include "Heightmap.hpp"

LDouble AdvancedHeightMap::getLowerLimit() const
{
	return lowerLimit;
}

const LDouble (& AdvancedHeightMap::getHeights() const)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE]
{
	return height;
}

void AdvancedHeightMap::setHeights(const LDouble (&arr)[TERRAIN_MAP_SIZE][TERRAIN_MAP_SIZE])
{
	std::memcpy(height, arr, sizeof(height));
}

Double AdvancedHeightMap::getHeight(const Int x, const Int y) const
{
	return height[x][y];
}

void AdvancedHeightMap::setLoaded(const Bool value)
{
	loaded = value;
}

void AdvancedHeightMap::setLowerLimit(const LDouble value)
{
	lowerLimit = value;
}

void AdvancedHeightMap::setUpperLimit(const LDouble value)
{
	upperLimit = value;
}

Bool AdvancedHeightMap::isLoaded() const
{
	return loaded;
}

LDouble AdvancedHeightMap::getUpperLimit() const
{
	return upperLimit;
}
