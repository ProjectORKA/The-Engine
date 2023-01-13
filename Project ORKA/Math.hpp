
#pragma once

#include "Basics.hpp"

#define PI 3.14159265359f

struct TiledRectangle;

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

Bool isOdd(ULL a);
Bool isEven(ULL a);
Bool withinLength(Vec3 a, Float b);
Bool withinDiamondArea(Vec3 a, Float b);
Bool isNear(Vec3 a, Vec3 b, Float error);
Bool isNear(Float a, Float b, Float error);
Bool pointInsideSphere(Vec3 point, Sphere sphere);
Bool pointInsideSpheres(Vec3 point, List<Sphere> spheres);
Bool pointInsideSpheres(Vec3 point, Vector<Sphere> spheres);
Bool isWithinDistanceOfOtherPoints(Vec2 point, Vector<Vec2>& points, Float dist);
Bool pointInsideSphereAtlocationWithRadius(Vec3 point, Vec3 position, Float radius);

UInt max(UInt a, UInt b);
UInt nextPowerOfTwo(UInt& value);
UInt countBitsInFlags(Short flags);
UInt fibonacciSequence(UInt iterations);

Index idOfClosestPoint(Vec2 origin, Vector<Vec2>& positions);
Index idOfClosestPointInLoopingSpace(Vec2 origin, Vector<Vec2>& positions, Float extend);

Int max(Int a, Int b);

Float sq(Float a);
Float min(Float a, Float b);
Float mod(Float a, Float b);
Float snap(Float a, Float b);
Float distance(Vec2 a, Vec2 b);
Float distance(Float a, Float b);
Float lerp(Float a, Float b, Float alpha);
Float clerp(Float a, Float b, Float alpha);
Float approach(Float input, Float maxValue);
Float deltaInLoopingSpace(Float a, Float b, Float extend);
Float getDistanceToClosestPoint(Vec3 point, Vector<Vec3>& points);
Float distanceToPointInLoopingSpace(Vec2 a, Vec2 b, Float extend);

LDouble dmod(LDouble x, LDouble y);
LDouble lerp(LDouble a, LDouble b, LDouble alpha);
LDouble clerp(LDouble a, LDouble b, LDouble alpha);

Vec2 vectorFromAToB(Vec2 a, Vec2 b);
Vec2 clerp(Vec2 a, Vec2 b, Float alpha);
Vec2 deltaInLoopingSpace(Vec2 a, Vec2 b, Float extend);

Vec3 max(Vec3 a, Vec3 b);
Vec3 projectToCube(Vec3 vec);
Vec3 vectorFromAToB(Vec3 a, Vec3 b);
Vec3 lerp(Vec3 a, Vec3 b, Float alpha);
Vec3 clerp(Vec3 a, Vec3 b, Float alpha);
Vec3 getClosestPoint(Vec3 point, List<Vec3>& points);
Vec3 quadraticInterpolation(Vec3 start, Vec3 control, Vec3 end, Float time);

Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);

Matrix matrixFromScale(Vec2 s);
Matrix matrixFromScale(Vec3 s);
Matrix matrixFromScale(Float size);
Matrix matrixFromLocation(Vec2 location);
Matrix matrixFromLocation(Vec3 location);
Matrix matrixFromOrientation(Orientation o);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z);
Matrix matrixFromTiledRectangle(TiledRectangle area);
Matrix matrixFromRotation(Float x, Float y, Float z);
Matrix matrixFromLocationAndSize2D(Vec2 pos, Vec2 size);
Matrix matrixFromPositionAndDirection(Vec2 pos, Vec2 dir);
Matrix matrixFromLocationAndSize(Vec4 compressedTransform);
Matrix matrixFromLocationAndSize(Vec3 location, Float size);
Matrix matrixFromLocationAndSize(Vec2 location, Float size);
Matrix matrixFromOrientation(Orientation o, Vec3 position, Float size);
Matrix matrixFromLocationAndSize2D(Float x, Float y, Float w, Float h);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z, Vec3 position, Float size);
Matrix matrixFromLocationDirectionAndSize(Vec2 pos, Vec2 dir, Float size);

void loopWithin(Vec2& point, Float extend);
void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius);
void getClosestPoint(Vec3 point, Vector<Vec3>& points, Index& closestID, Vec3& closestPoint);
void spaceColonization(Vector<Vec3>& points, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius);

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
	a = clerp(a, b, rateOfChange);
}