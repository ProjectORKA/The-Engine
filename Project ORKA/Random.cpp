#include "Random.hpp"
#include <iostream>
#include <random>
#include <limits>

ULong xorshf96() {
	static ULong x = 123456789, y = 362436069, z = 521288629;
	ULong t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}

Vec3 randomVec3() { return Vec3(randomFloat(), randomFloat(), randomFloat()); }
Vec2 randomVec2() { return Vec2(randomFloat(), randomFloat()); }
Float randomFloat() { return randomFloat(0, 1); }

void randomizeSeed() {
	const UInt seed = now().time_since_epoch().count();
	srand(seed);
	logDebug(String("Seed:").append(std::to_string(seed)));
}

ULL random(const ULL size) {
	if (size == 0) return 0;

	std::random_device seed;
	std::mt19937_64 randomizer(seed());
	std::uniform_int_distribution<unsigned long long> distribution;

	return distribution(randomizer) / (ULLONG_MAX / size);
}

ULL random(const ULL min, const ULL max) {
	const ULL range = max - min;

	if (range == 0) return 0;

	std::random_device seed;
	std::mt19937_64 randomizer(seed());
	std::uniform_int_distribution<unsigned long long> distribution;

	return distribution(randomizer) / (ULLONG_MAX / range) + min;
}

Vec2 randomPointInCircle(const Float radius) {
	Vec2 v;
	do { v = randomVec2(-radius, radius); }
	while (length(v) > radius);
	return v;
}

Vec3 randomUnitVec3() { return normalize(randomPointInSphere(1)); }
Vec2 randomUnitVec2() { return normalize(randomPointInCircle(1)); }

UInt randomUInt(const UInt max) {
	if (max < 1) return 0;
	return xorshf96() % max;
}

Int randomInt(const Int max) {
	if (max < 1) return 0;
	return xorshf96() % max;
}

Vec3 randomVec3(const Vec3 high) { return Vec3(randomFloat(high.x), randomFloat(high.y), randomFloat(high.z)); }
Vec2 randomVec2(const Float high) { return Vec2(randomFloat(high), randomFloat(high)); }
Vec3 randomVec3(const Float high) { return Vec3(randomFloat(high), randomFloat(high), randomFloat(high)); }
Float randomFloat(const Float high) { return randomFloat(0, high); }

Vec3 randomVec3(const Vec3 low, const Vec3 high) {
	return Vec3(randomFloat(low.x, high.x), randomFloat(low.y, high.y), randomFloat(low.z, high.z));
}

Vec4 randomVec4(const Float low, const Float high) {
	return Vec4(randomFloat(low, high), randomFloat(low, high), randomFloat(low, high), randomFloat(low, high));
}

Vec3 randomPointInSphere(const Float radius) {
	Vec3 v;
	do { v = randomVec3(-radius, radius); }
	while (length(v) > radius);
	return v;
};
Vec3 randomPointOnSphere(const Float radius) { return normalize(randomPointInSphere(1)) * radius; }

Vec3 randomVec3(const Float low, const Float high) {
	return Vec3(randomFloat(low, high), randomFloat(low, high), randomFloat(low, high));
}

Vec2 randomVec2(const Float low, const Float high) { return Vec2(randomFloat(low, high), randomFloat(low, high)); }

Float randomFloat(const Float low, const Float high) {
	return low + (static_cast<Float>(xorshf96()) / static_cast<Float>(ULONG_MAX) * (high - low));
}

Vec3 randomPointOnSphereAtLocation(const Vec4 sphere) { return randomPointOnSphere(sphere.w) + Vec3(sphere); }

Vec3 randomPointOnSphereAtLocation(const Float radius, const Vec3 location) {
	return randomPointOnSphere(radius) + location;
}
