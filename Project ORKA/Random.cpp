#include "Random.hpp"
#include <random>
#include "Math.hpp"
#include "Time.hpp"

using RandomDevice = std::random_device;

// FAST:

//64 random bits
ULL randomULLFast()
{
	// original seed values
	//static ULL x = 123456789;
	//static ULL y = 362436069;
	//static ULL z = 521288629;

	//updated seed values to produce accurate randoms for ULL
	static ULL x = 10596601668567;
	static ULL y = 60441451194812;
	static ULL z = 34516354845907;

	ULL t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

ULL randomULLFast(const ULL max)
{
	if(max < 1) return 0;
	return randomULLFast() % max;
}

Vec3 randomVec3Fast()
{
	return {randomFloatFast(), randomFloatFast(), randomFloatFast()};
}

Vec2 randomVec2Fast()
{
	return {randomFloatFast(), randomFloatFast()};
}

Float randomFloatFast()
{
	return randomFloatFast(0.0f, 1.0f);
}

Double randomDoubleFast()
{
	return randomDoubleFast(0.0, 1.0);
}

Vec3 randomUnitVec3Fast()
{
	return normalize(randomPointInSphereFast(1));
}

Vec2 randomUnitVec2Fast()
{
	return normalize(randomPointInCircleFast(1));
}

Int randomIntFast(const Int max)
{
	if(max < 1) return 0;
	return mod(static_cast<Int>(randomULLFast()), max);
}

Int randomIntFast(const UInt max)
{
	if(max < 1) return 0;
	return static_cast<Int>(randomULLFast() % static_cast<ULL>(max));
}

Int randomIntFast(const Int min, const Int max)
{
	//return (randomULLFast() % static_cast<ULL>(max - min));

	const ULL range       = static_cast<ULL>(max) - min + 1;
	const ULL randomValue = randomULLFast() % range;
    return static_cast<Int>(randomValue) + min;
}

Int randomIntFast(const ULL max)
{
	if(max < 1) return 0;
	return static_cast<Int>(randomULLFast() % static_cast<ULL>(max));
}

UInt randomUIntFast(const UInt max)
{
	if(max < 1) return 0;
	return randomULLFast() % max;
}

Vec3 randomVec3Fast(const Vec3 max)
{
	return {randomFloatFast(max.x), randomFloatFast(max.y), randomFloatFast(max.z)};
}

Vec2 randomVec2Fast(const Float max)
{
	return {randomFloatFast(max), randomFloatFast(max)};
}

Vec3 randomVec3Fast(const Float max)
{
	return {randomFloatFast(max), randomFloatFast(max), randomFloatFast(max)};
}

Float randomFloatFast(const Float max)
{
	return randomFloatFast(0, max);
}

Vec2 randomPointInCircleFast(const Float radius)
{
	Vec2 v;
	do v = randomVec2Fast(-radius, radius);
	while(length(v) > radius);
	return v;
}

Vec3 randomPointInSphereFast(const Float radius)
{
	Vec3 v;
	do v = randomVec3Fast(-radius, radius);
	while(length(v) > radius);
	return v;
}

Vec3 randomPointOnSphereFast(const Float radius)
{
	return normalize(randomPointInSphereFast(1)) * radius;
}

Vec3 randomVec3Fast(const Vec3 min, const Vec3 max)
{
	return {randomFloatFast(min.x, max.x), randomFloatFast(min.y, max.y), randomFloatFast(min.z, max.z)};
}

Vec2 randomVec2Fast(const Float min, const Float max)
{
	return {randomFloatFast(min, max), randomFloatFast(min, max)};
}

Vec4 randomVec4Fast(const Float min, const Float max)
{
	return {randomFloatFast(min, max), randomFloatFast(min, max), randomFloatFast(min, max), randomFloatFast(min, max)};
}

Vec3 randomVec3Fast(const Float min, const Float max)
{
	return {randomFloatFast(min, max), randomFloatFast(min, max), randomFloatFast(min, max)};
}

Float randomFloatFast(const Float min, const Float max)
{
	return randomFloatFast(static_cast<Double>(min), static_cast<Double>(max));
}

Float randomFloatFast(const Double min, const Double max)
{
	return static_cast<Float>(static_cast<Double>(min) + static_cast<Double>(randomULLFast()) / static_cast<Double>(ULLONG_MAX) * (max - min));
}

Double randomDoubleFast(const Double min, const Double max)
{
	return static_cast<Float>(min + static_cast<Double>(randomULLFast()) / static_cast<Double>(ULLONG_MAX) * (max - min));
}

Vec3 randomPointOnSphereAtLocationFast(const Vec4 sphere)
{
	return randomPointOnSphereFast(sphere.w) + Vec3(sphere);
}

Vec3 randomPointOnSphereAtLocationFast(const Float radius, const Vec3 location)
{
	return randomPointOnSphereFast(radius) + location;
}

Vec3 randomVec3Fast(const Float xMin, const Float xMax, const Float yMin, const Float yMax, const Float zMin, const Float zMax)
{
	return {randomFloatFast(xMin, xMax), randomFloatFast(yMin, yMax), randomFloatFast(zMin, zMax)};
}

// SLOW:
RandomDevice seed;

ULL randomULLSlow()
{
	std::mt19937_64                    randomizer(seed());
	std::uniform_int_distribution<ULL> distribution;
	return distribution(randomizer);
}

Int randomIntSlow()
{
	return static_cast<Int>(randomULLFast());
}

void randomizeSeed()
{
	srand(static_cast<UInt>(now().time_since_epoch().count()));
}

UInt randomUIntFast()
{
	return randomULLFast();
}

Vec3 randomVec3Slow()
{
	return {randomFloatSlow(), randomFloatSlow(), randomFloatSlow()};
}

Float randomFloatSlow()
{
	return randomFloatSlow(0.0f, 1.0f);
}

Int randomIntSlow(const Int max)
{
	if(max < 1) return 0;
	return mod(static_cast<Int>(randomULLSlow()), max);
}

ULL randomULLSlow(const ULL max)
{
	if(max == 0) return 0;

	return randomULLSlow() / (ULLONG_MAX / max);
}

Int randomIntSlow(const SizeT max)
{
	if(max < 1) return 0;
	return mod(static_cast<Int>(randomULLSlow()), static_cast<Int>(max));
}

ULL randomULLSlow(const ULL min, const ULL max)
{
	const ULL range = max - min;

	if(range == 0) return 0;

	std::random_device                 seed;
	std::mt19937_64                    randomizer(seed());
	std::uniform_int_distribution<ULL> distribution;

	return distribution(randomizer) / (ULLONG_MAX / range) + min;
}

Float randomFloatSlow(const Float min, const Float max)
{
	return randomFloatSlow(static_cast<Double>(min), static_cast<Double>(max));
}

Float randomFloatSlow(const Double min, const Double max)
{
	return static_cast<Float>(static_cast<Double>(min) + static_cast<Double>(randomULLSlow()) / static_cast<Double>(ULLONG_MAX) * (max - min));
}

Double randomDoubleSlow(const Double min, const Double max)
{
	return static_cast<Double>(min) + static_cast<Double>(randomULLSlow()) / static_cast<Double>(ULLONG_MAX) * (max - min);
}