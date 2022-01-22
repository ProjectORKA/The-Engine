
#include "Random.hpp"
#include "Time.hpp"
#include "Debug.hpp"

Vec3 randomVec3() {
	return Vec3(randomFloat(), randomFloat(), randomFloat());
}
Vec2 randomVec2() {
	return Vec2(randomFloat(), randomFloat());
}
Float randomFloat()
{
	return randomFloat(0, 1);
}
void randomizeSeed() {
	UInt seed = now().time_since_epoch().count();
	srand(seed);
	logDebug(String("Seed:").append(std::to_string(seed)));
}
ULL random(ULL size) {
	std::random_device seed;
	std::mt19937_64 randomizer(seed());
	std::uniform_int_distribution<unsigned long long> distribution;

	return distribution(randomizer) / (ULLONG_MAX / size);
}
Vec3 randomUnitVec3() {
	return normalize(randomPointInSphere(1));
}
Int randomInt(Int max) {
	return rand() % max;
}
Vec3 randomVec3(Vec3 high) {
	return Vec3(randomFloat(high.x), randomFloat(high.y), randomFloat(high.z));
}
Vec2 randomVec2(Float high) {
	return Vec2(randomFloat(high), randomFloat(high));
}
Vec3 randomVec3(Float high) {
	return Vec3(randomFloat(high), randomFloat(high), randomFloat(high));
}
Float randomFloat(Float high) {
	return randomFloat(0, high);
}
Vec3 randomPointInSphere(Float radius) {
	Vec3 v;
	do {
		v = randomVec3(-radius, radius);
	} while (length(v) > radius);
	return v;
};
Vec3 randomPointOnSphere(Float radius) {
	return normalize(randomPointInSphere(1)) * radius;
}
Vec3 randomVec3(Float low, Float high) {
	return Vec3(randomFloat(low, high), randomFloat(low, high), randomFloat(low, high));
}
Vec2 randomVec2(Float low, Float high) {
	return Vec2(randomFloat(low, high), randomFloat(low, high));
}
Float randomFloat(Float low, Float high)
{
	return low + static_cast <Float> (rand()) / (static_cast <Float> (RAND_MAX / (high - low)));
}
Vec3 randomPointOnSphereAtLocation(Float radius, Vec3 location) {
	return randomPointOnSphere(radius) + location;
}

Vec3 randomPointOnSphereAtLocation(Vec4 sphere) {
	return randomPointOnSphere(sphere.w) + Vec3(sphere);
}