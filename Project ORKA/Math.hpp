#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f

template<typename T>
T sq(T a) {
	return (a * a);
}

template<typename T>
T max(T a, T b) {
	return (a < b) ? b : a;
}

template <typename T>
T min(T a, T b) {
	return !(b < a) ? a : b;
}

template <typename T>
T clamp(T a, T min, T max) {
	if (a > max) return max;
	if (a < min) return min;
	return a;
}

Float distance(Float a, Float b);

Float distance(Vec2 a, Vec2 b);

Float lerp(Float a, Float b, Float alpha);
bool isFloatNearOther(Float a, Float b, Float error);
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);

UInt nextPowerOfTwo(UInt& value);

Vec3 projectToCube(Vec3 vec);

struct Graph {
	Vector<Float> points;

	void add(Float value);
};


