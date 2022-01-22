#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f

struct Graph {
	Vector<Float> points;

	void add(Float value);
};

struct Orientation {
	Vec3 x = Vec3(1, 0, 0);
	Vec3 y = Vec3(0, 1, 0);
	Vec3 z = Vec3(0, 0, 1);

	Orientation(Vec3 direction);
	Orientation(Vec3 direction, Vec3 up);
};

Float min(Float a, Float b);
Float mod(Float a, Float b);
Vec3 projectToCube(Vec3 vec);
Float snap(Float a, Float b);
Float distance(Vec2 a, Vec2 b);
Float distance(Float a, Float b);
UInt nextPowerOfTwo(UInt& value);
Vec3 lerp(Vec3 a, Vec3 b, Float alpha);
Matrix matrixFromLocation(Vec2 location);
Matrix matrixFromLocation(Vec3 location);
Float lerp(Float a, Float b, Float alpha);
Matrix matrixFromOrientation(Orientation o);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z);
bool isFloatNearOther(Float a, Float b, Float error);
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);
Matrix matrixFromLocationAndSize(Vec4 compressedTransform);
Matrix matrixFromLocationAndSize(Vec3 location, Float size);
Matrix matrixFromOrientation(Orientation o, Vec3 position, Float size);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z, Vec3 position, Float size);



template<typename T>
T sq(T a) {
	return (a * a);
}

template<typename T, typename R>
T max(T a, R b) {
	return (a < b) ? b : a;
}

template <typename T, typename R>
T min(T a, R b) {
	return !(b < a) ? a : b;
}

template <typename T, typename R, typename E>
T clamp(T a, R min, E max) {
	if (a > max) return max;
	if (a < min) return min;
	return a;
}

template <typename T>
void approach(T& a, T b, Float rateOfChange) {
	a = lerp(a, b, rateOfChange);
}