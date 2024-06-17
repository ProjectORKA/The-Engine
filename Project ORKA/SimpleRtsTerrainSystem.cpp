#include "SimpleRTSTerrainSystem.hpp"
#include "PerlinNoise.hpp"

Float SimpleRtsTerrainSystem::simpleRtsTerrainFunction(const Vec2 position)
{
	const Float height = 15.0f * static_cast<Float>(noise.octaveNoise0_1(position.x, position.y, 16.0f));
	return height;
}
