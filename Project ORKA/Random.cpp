#include "Random.hpp"
#include <random>
#include "Math.hpp"
#include "Time.hpp"

std::random_device seed;

ULong xorshf96()
{
	static ULong x = 123456789, y = 362436069, z = 521288629;
	ULong        t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

Vec3 randomVec3()
{
	return {randomFloat(), randomFloat(), randomFloat()};
}

Vec2 randomVec2()
{
	return {randomFloat(), randomFloat()};
}

Float randomFloat()
{
	return randomFloat(0, 1);
}

void randomizeSeed()
{
	srand(static_cast<UInt>(now().time_since_epoch().count()));
}

Vec3 randomUnitVec3()
{
	return normalize(randomPointInSphere(1));
}

Vec2 randomUnitVec2()
{
	return normalize(randomPointInCircle(1));
}

ULL random(const ULL size)
{
	if(size == 0) return 0;

	std::mt19937_64                    randomizer(seed());
	std::uniform_int_distribution<ULL> distribution;

	return distribution(randomizer) / (ULLONG_MAX / size);
}

Int randomInt(const Int max)
{
	if(max < 1) return 0;
	return mod(static_cast<Int>(xorshf96()), max);
}

UInt randomUInt(const UInt max)
{
	if(max < 1) return 0;
	return xorshf96() % max;
}

Vec3 randomVec3(const Vec3 max)
{
	return {randomFloat(max.x), randomFloat(max.y), randomFloat(max.z)};
}

Vec2 randomVec2(const Float max)
{
	return {randomFloat(max), randomFloat(max)};
}

Vec3 randomVec3(const Float max)
{
	return {randomFloat(max), randomFloat(max), randomFloat(max)};
}

Float randomFloat(const Float max)
{
	return randomFloat(0, max);
}

ULL random(const ULL min, const ULL max)
{
	const ULL range = max - min;

	if(range == 0) return 0;

	std::random_device                 seed;
	std::mt19937_64                    randomizer(seed());
	std::uniform_int_distribution<ULL> distribution;

	return distribution(randomizer) / (ULLONG_MAX / range) + min;
}

Vec2 randomPointInCircle(const Float radius)
{
	Vec2 v;
	do v = randomVec2(-radius, radius);
	while(length(v) > radius);
	return v;
}

Vec3 randomPointInSphere(const Float radius)
{
	Vec3 v;
	do v = randomVec3(-radius, radius);
	while(length(v) > radius);
	return v;
}

Vec3 randomPointOnSphere(const Float radius)
{
	return normalize(randomPointInSphere(1)) * radius;
}

Vec3 randomVec3(const Vec3 min, const Vec3 max)
{
	return {randomFloat(min.x, max.x), randomFloat(min.y, max.y), randomFloat(min.z, max.z)};
}

Vec4 randomVec4(const Float min, const Float max)
{
	return {randomFloat(min, max), randomFloat(min, max), randomFloat(min, max), randomFloat(min, max)};
}

Vec3 randomVec3(const Float min, const Float max)
{
	return {randomFloat(min, max), randomFloat(min, max), randomFloat(min, max)};
}

Vec2 randomVec2(const Float min, const Float max)
{
	return {randomFloat(min, max), randomFloat(min, max)};
}

Float randomFloat(const Float min, const Float max)
{
	return min + static_cast<Float>(xorshf96()) / static_cast<Float>(ULONG_MAX) * (max - min);
}

Vec3 randomPointOnSphereAtLocation(const Vec4 sphere)
{
	return randomPointOnSphere(sphere.w) + Vec3(sphere);
}

Vec3 randomPointOnSphereAtLocation(const Float radius, const Vec3 location)
{
	return randomPointOnSphere(radius) + location;
}
