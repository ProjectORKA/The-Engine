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

[[nodiscard]] Bool isOdd(ULL a);
[[nodiscard]] Bool isEven(ULL a);
[[nodiscard]] Bool withinLength(Vec3 a, Float b);
[[nodiscard]] Bool withinDiamondArea(Vec3 a, Float b);
[[nodiscard]] Bool isNear(Vec3 a, Vec3 b, Float error);
[[nodiscard]] Bool isNear(Float a, Float b, Float error);

//angles
[[nodiscard]] Bool compareByAngle(const Vec2& a, const Vec2& b);

//lines
[[nodiscard]] Bool linesIntersecting(Vec2 a, Vec2 b, Vec2 c, Vec2 d);

//points
[[nodiscard]] Bool pointInsideSphere(Vec3 point, Sphere sphere);
[[nodiscard]] Bool pointInsideSpheres(Vec3 point, List<Sphere> spheres);
[[nodiscard]] Bool pointInsideSpheres(Vec3 point, const Vector<Sphere>& spheres);
[[nodiscard]] Bool pointInsideSphereAtPositionWithRadius(Vec3 point, Vec3 position, Float radius);
[[nodiscard]] Bool pointWithinDistanceOfOtherPoints(Vec2 point, const Vector<Vec2>& points, Float dist);

//circles
[[nodiscard]] Bool circleCollidingWithCircles(Vec2 pos, Float radius, const Vector<Vec2>& circles, Float pointsRadius);

[[nodiscard]] UInt max(UInt a, UInt b);
[[nodiscard]] UInt nextPowerOfTwo(UInt value);
[[nodiscard]] UInt countBitsInFlags(UInt flags);
[[nodiscard]] UInt fibonacciSequence(UInt iterations);

[[nodiscard]] Index xyToIndex(Int x, Int y, Int size);
[[nodiscard]] Index xyToIndex(Int x, Int y, Int width, Int height);
[[nodiscard]] Index idOfClosestPoint(Vec2 origin, const Vector<Vec2>& positions);
[[nodiscard]] Index idOfClosestPointInLoopingSpace(Vec2 origin, const Vector<Vec2>& positions, Float extend);
[[nodiscard]] Index idOfClosestPoint(Vec2 origin, const Vector<Index>& indices, const Vector<Vec2>& positions);

[[nodiscard]] Int max(Int a, Int b);
[[nodiscard]] Int mod(Int a, Int b);

[[nodiscard]] Float sq(Float a);
[[nodiscard]] Float min(Float a, Float b);
[[nodiscard]] Float mod(Float a, Float b);
[[nodiscard]] Float snap(Float a, Float b);
[[nodiscard]] Float distance(Vec2 a, Vec2 b);
[[nodiscard]] Float distance(Float a, Float b);
[[nodiscard]] Float lerp(Float a, Float b, Float alpha);
[[nodiscard]] Float clerp(Float a, Float b, Float alpha);
[[nodiscard]] Float approach(Float input, Float maxValue);
[[nodiscard]] Float angleBetweenAAndB(const Vec3& a, const Vec3& b);
[[nodiscard]] Float deltaInLoopingSpace(Float a, Float b, Float extend);
[[nodiscard]] Float distanceToPointInLoopingSpace(Vec2 a, Vec2 b, Float extend);
[[nodiscard]] Float getDistanceToClosestPoint(Vec3 point, const Vector<Vec3>& points);

[[nodiscard]] Double mod(Double a, Double b);

[[nodiscard]] LDouble dmod(LDouble x, LDouble y);
[[nodiscard]] LDouble lerp(LDouble a, LDouble b, LDouble alpha);
[[nodiscard]] LDouble clerp(LDouble a, LDouble b, LDouble alpha);

[[nodiscard]] Vec2 vectorFromAToB(Vec2 a, Vec2 b);
[[nodiscard]] Vec2 clerp(Vec2 a, Vec2 b, Float alpha);
[[nodiscard]] Vec2 deltaInLoopingSpace(Vec2 a, Vec2 b, Float extend);

[[nodiscard]] Vec3 normalize(Vec3 a);
[[nodiscard]] Vec3 max(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 projectToCube(Vec3 vec);
[[nodiscard]] Vec3 vectorFromAToB(Vec3 a, Vec3 b);
[[nodiscard]] Vec3 lerp(Vec3 a, Vec3 b, Float alpha);
[[nodiscard]] Vec3 clerp(Vec3 a, Vec3 b, Float alpha);
[[nodiscard]] Vec3 getClosestPoint(Vec3 point, const List<Vec3>& points);
[[nodiscard]] Vec2 getClosestPoint(Vec2 point, const Vector<Vec2>& points);
[[nodiscard]] Vec3 quadraticInterpolation(Vec3 start, Vec3 control, Vec3 end, Float time);

[[nodiscard]] UllVec4 getBoundingBoxIds(const Vector<Vec2>& points);

[[nodiscard]] Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest);

[[nodiscard]] Matrix matrixFromSize(Vec2 s);
[[nodiscard]] Matrix matrixFromSize(Vec3 s);
[[nodiscard]] Matrix matrixFromSize(Float size);
[[nodiscard]] Matrix matrixFromPosition(Vec2 position);
[[nodiscard]] Matrix matrixFromPosition(Vec3 position);
[[nodiscard]] Matrix matrixFromPosition(Float x, Float y);
[[nodiscard]] Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z);
[[nodiscard]] Matrix clerp(Matrix a, Matrix b, Float alpha);
[[nodiscard]] Matrix matrixFromOrientation(const Orientation& o);
[[nodiscard]] Matrix screenSpaceMatrix(Float width, Float height);
[[nodiscard]] Matrix matrixFromPosition(Float x, Float y, Float z);
[[nodiscard]] Matrix matrixFromTiledRectangle(TiledRectangle area);
[[nodiscard]] Matrix matrixFromRotation(Float x, Float y, Float z);
[[nodiscard]] Matrix matrixFromPositionAndSize(Vec2 pos, Vec2 size);
[[nodiscard]] Matrix matrixFromPositionAndDirection(Vec2 pos, Vec2 dir);
[[nodiscard]] Matrix matrixFromPositionAndSize(Vec4 compressedTransform);
[[nodiscard]] Matrix matrixFromPositionAndSize(Vec3 position, Float size);
[[nodiscard]] Matrix matrixFromPositionAndSize(Vec2 position, Float size);
[[nodiscard]] Matrix matrixFromDirectionAndPosition(Vec3 direction, Vec3 position);
[[nodiscard]] Matrix matrixFromPositionAndSize(Float x, Float y, Float w, Float h);
[[nodiscard]] Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z, Vec3 position, Float size);
[[nodiscard]] Matrix matrixFromPositionDirectionAndSize(Vec2 pos, Vec2 dir, Float size);
[[nodiscard]] Matrix matrixFromOrientation(const Orientation& o, Vec3 position, Float size);

[[nodiscard]] Vector<Index> shuffledIndices(UInt size);

[[nodiscard]] Vector<Vec3> circleOfPoints(Float radius, UInt amount);
[[nodiscard]] Vector<Vec3> vec2VectorToVec3Vector(const Vector<Vec2>& vec2Vector);
[[nodiscard]] Vector<Vec2> vec3VectorToVec2Vector(const Vector<Vec3>& vec3Vector);

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

template <typename T>
T max(T a, T b)
{
	return a < b ? b : a;
}

template <typename T>
T min(T a, T b)
{
	return !(b < a) ? a : b;
}

template <typename T>
T clamp(T a, T min, T max)
{
	if (a > max) return max;
	if (a < min) return min;
	return a;
}

template <typename T>
void approach(T& a, T b, Float rateOfChange)
{
	a = clerp(a, b, rateOfChange);
}
