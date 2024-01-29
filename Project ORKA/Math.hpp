#pragma once

#include "Basics.hpp"

#define PI 3.14159265358979323846f

struct TiledRectangle;

struct Line2D
{
	Vec2 start;
	Vec2 end;
};

struct Line3D
{
	Vec3 start = Vec3(0);
	Vec3 end   = Vec3(0);

	Line3D(Vec3 start, Vec3 end);
	Line3D(Vec2 start, Vec2 end);
};

struct Graph
{
	Vector<Float> points;

	void add(Float value);
};

struct Orientation
{
	Vec3 x = Vec3(1, 0, 0);
	Vec3 y = Vec3(0, 1, 0);
	Vec3 z = Vec3(0, 0, 1);

	explicit Orientation(Vec3 direction);
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
Bool pointInsideSpheres(Vec3 point, const Vector<Sphere>& spheres);
Bool pointInsideSphereAtPositionWithRadius(Vec3 point, Vec3 position, Float radius);
Bool isWithinDistanceOfOtherPoints(Vec2 point, const Vector<Vec2>& points, Float dist);

UInt max(UInt a, UInt b);
UInt nextPowerOfTwo(UInt value);
UInt countBitsInFlags(UInt flags);
UInt fibonacciSequence(UInt iterations);

Index idOfClosestPoint(Vec2 origin, const Vector<Vec2>& positions);
Index idOfClosestPointInLoopingSpace(Vec2 origin, const Vector<Vec2>& positions, Float extend);

Int max(Int a, Int b);
Int mod(Int a, Int b);

Float sq(Float a);
Vec3  normalize(Vec3 a);
Float min(Float a, Float b);
Float mod(Float a, Float b);
Float snap(Float a, Float b);
Float distance(Vec2 a, Vec2 b);
Float distance(Float a, Float b);
Float lerp(Float a, Float b, Float alpha);
Float clerp(Float a, Float b, Float alpha);
Float approach(Float input, Float maxValue);
Float angleBetweenAAndB(const Vec3& a, const Vec3& b);
Float deltaInLoopingSpace(Float a, Float b, Float extend);
Float distanceToPointInLoopingSpace(Vec2 a, Vec2 b, Float extend);
Float getDistanceToClosestPoint(Vec3 point, const Vector<Vec3>& points);

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
Vec3 getClosestPoint(Vec3 point, const List<Vec3>& points);
Vec3 quadraticInterpolation(Vec3 start, Vec3 control, Vec3 end, Float time);

ULLVec4 getBoundingBoxIds(const Vector<Vec2>& points);

Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);

Matrix matrixFromSize(Vec2 s);
Matrix matrixFromSize(Vec3 s);
Matrix matrixFromSize(Float size);
Matrix matrixFromPosition(Vec2 location);
Matrix matrixFromPosition(Vec3 location);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z);
Matrix clerp(Matrix a, Matrix b, Float alpha);
Matrix matrixFromOrientation(const Orientation& o);
Matrix screenSpaceMatrix(Float width, Float height);
Matrix matrixFromTiledRectangle(TiledRectangle area);
Matrix matrixFromRotation(Float x, Float y, Float z);
Matrix matrixFromPositionAndSize(Vec2 pos, Vec2 size);
Matrix matrixFromPositionAndDirection(Vec2 pos, Vec2 dir);
Matrix matrixFromPositionAndSize(Vec4 compressedTransform);
Matrix matrixFromPositionAndSize(Vec3 location, Float size);
Matrix matrixFromPositionAndSize(Vec2 location, Float size);
Matrix matrixFromDirectionAndLocation(Vec3 direction, Vec3 location);
Matrix matrixFromPositionAndSize(Float x, Float y, Float w, Float h);
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z, Vec3 position, Float size);
Matrix matrixFromPositionDirectionAndSize(Vec2 pos, Vec2 dir, Float size);
Matrix matrixFromOrientation(const Orientation& o, Vec3 position, Float size);

Vector<Vec3> vec2VectorToVec3Vector(const Vector<Vec2>& vec2Vector);

[[nodiscard]] Vector<Matrix> matrixArrayFromPositions(const Vector<Vec3>& positions);
[[nodiscard]] Vector<Matrix> matrixArrayFromPositions(const Vector<Vec2>& positions);
[[nodiscard]] Vector<Matrix> matrixArrayFromCompactVec4(const Vector<Vec4>& compactTransform);
[[nodiscard]] Vector<Matrix> matrixArrayFromPositionsAndSize(const Vector<Vec2>& positions, Float size);
[[nodiscard]] Vector<Matrix> matrixArrayFromPositionsAndSize(const Vector<Vec3>& positions, Float size);
[[nodiscard]] Vector<Matrix> matrixArrayFromPositionsAndDirections(const Vector<Vec2>& pos, const Vector<Vec2>& dir);
[[nodiscard]] Vector<Matrix> matrixArrayFromPositionsDirectionsAndSizes(const Vector<Vec2>& position, const Vector<Vec2>& direction, const Vector<Float>& size);

void loopWithinCentered(Vec2& point, Float extend);
void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius);
void getClosestPoint(Vec3 point, const Vector<Vec3>& points, Index& closestId, Vec3& closestPoint);
void spaceColonization(Vector<Vec3>& points, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius);

template <typename T> T max(T a, T b)
{
	return a < b ? b : a;
}

template <typename T> T min(T a, T b)
{
	return !(b < a) ? a : b;
}

template <typename T> T clamp(T a, T min, T max)
{
	if(a > max) return max;
	if(a < min) return min;
	return a;
}

template <typename T> void approach(T& a, T b, Float rateOfChange)
{
	a = clerp(a, b, rateOfChange);
}
