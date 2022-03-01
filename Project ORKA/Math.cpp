
#include "Math.hpp"
#include "TiledMath.hpp"

Bool isOdd(ULL a) {
	return a % 2 != 0;
}
Bool isEven(ULL a) {
	return a % 2 == 0;
}
Float min(Float a, Float b) {
	if (a < b) return a;
	else return b;
}
Float mod(Float a, Float b) {
	return fmod(a, b);
}
Vec3 projectToCube(Vec3 vec) {
	Float max = abs(vec.x);
	if (abs(vec.y) > max) max = abs(vec.y);
	if (abs(vec.z) > max) max = abs(vec.z);
	return vec / Vec3(max);
}
void Graph::add(Float value) {
	points.push_back(value);
}
Float snap(Float a, Float b) {
	return round(a/b)*b;
}
Float distance(Vec2 a, Vec2 b) {
	return glm::distance(a, b);
}
Matrix matrixFromScale(Vec3 s) {
	Matrix m;
	m[0] = Vec4(s.x, 0, 0, 0);
	m[1] = Vec4(0, s.y, 0, 0);
	m[2] = Vec4(0, 0, s.z, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}
Float distance(Float a, Float b) {
	return abs(a - b);
}
UInt nextPowerOfTwo(UInt& value)
{
	UInt powerOfTwo = 1;
	while (powerOfTwo < value) powerOfTwo <<= 1;
	return powerOfTwo;
}
Bool withinLength(Vec3 a, Float b) {
	return (a.x* a.x + a.y * a.y + a.z * a.z) < (b * b);
};
LDouble dmod(LDouble x, LDouble y) {
	return x - (ULL)(x / y) * y; //[TODO] check if it actually works
}
Vec3 lerp(Vec3 a, Vec3 b, Float alpha) {
	Vec3 c;
	c.x = lerp(a.x, b.x, alpha);
	c.y = lerp(a.y, b.y, alpha);
	c.z = lerp(a.z, b.z, alpha);
	return c;
}
Bool withinDiamondArea(Vec3 a, Float b) {
	return (abs(a.x) + abs(a.y) + abs(a.z)) < 2 * b;
};
Orientation::Orientation(Vec3 direction) {
	//this function just assumes +Z is up
	z = normalize(direction);
	x = cross(normalize(z), Vec3(0, 0, 1));
	y = cross(normalize(z), x);
}
Matrix matrixFromLocation(Vec2 location) {
	Matrix m(1);
	m[3] = Vec4(location, 0, 1);
	return m;
}
Matrix matrixFromLocation(Vec3 location) {
	Matrix m(1);
	m[3] = Vec4(location, 1);
	return m;
}
Float lerp(Float a, Float b, Float alpha)
{
	alpha = clamp(alpha,0,1);
	return (a * (1 - alpha)) + (b * alpha);
}
Matrix matrixFromOrientation(Orientation o) {
	return matrixFromAxis(o.x, o.y, o.z);
}
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z) {
	Matrix m;
	m[0] = Vec4(x, 0);
	m[1] = Vec4(y, 0);
	m[2] = Vec4(z, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}
LDouble lerp(LDouble a, LDouble b, LDouble alpha)
{
	alpha = clamp(alpha, 0, 1);
	return (a * (1 - alpha)) + (b * alpha);
}
Orientation::Orientation(Vec3 direction, Vec3 up) {
	z = normalize(direction);
	x = cross(normalize(z), up);
	y = cross(normalize(z), x);
}
bool isFloatNearOther(Float a, Float b, Float error) {
	return fabsf(a - b) < error;
}
Matrix matrixFromTiledRectangle(TiledRectangle area) {
	Matrix m;
	m[0] = Vec4(area.size.x,		0,					0, 0);
	m[1] = Vec4(0,					area.size.y,		0, 0);
	m[2] = Vec4(0,					0,					1, 0);
	m[3] = Vec4(area.position.x,	area.position.y,	0, 1);
	return m;
}
Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest) { //copy of opengl tutorial
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	Vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(Vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(Vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return Rotation(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}
Matrix matrixFromLocationAndSize(Vec4 compressedTransform)
{
	Matrix m(compressedTransform.w);
	m[3] = compressedTransform;
	m[3].w = 1;
	return m;
}
Matrix matrixFromLocationAndSize(Vec3 location, Float size)
{
	Matrix m(size);
	m[3] = Vec4(location, 1);
	return m;
}

Matrix matrixFromOrientation(Orientation o, Vec3 position, Float size) {
	return matrixFromAxis(o.x, o.y, o.z, position, size);
}
Matrix matrixFromAxis(Vec3 x, Vec3 y, Vec3 z, Vec3 position, Float size) {
	Matrix m;
	m[0] = Vec4(size * x, 0);
	m[1] = Vec4(size * y, 0);
	m[2] = Vec4(size * z, 0);
	m[3] = Vec4(position, 1);
	return m;
}