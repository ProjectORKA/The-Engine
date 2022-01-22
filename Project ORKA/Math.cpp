
#include "Math.hpp"

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
Float distance(Float a, Float b) {
	return abs(a - b);
}
UInt nextPowerOfTwo(UInt& value)
{
	UInt powerOfTwo = 1;
	while (powerOfTwo < value) powerOfTwo <<= 1;
	return powerOfTwo;
}
Vec3 lerp(Vec3 a, Vec3 b, Float alpha) {
	Vec3 c;
	c.x = lerp(a.x, b.x, alpha);
	c.y = lerp(a.y, b.y, alpha);
	c.z = lerp(a.z, b.z, alpha);
	return c;
}
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
Float lerp(Float from, Float to, Float t)
{
	t = clamp(t,0,1);
	return (from * (1 - t)) + (to * t);
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
Orientation::Orientation(Vec3 direction, Vec3 up) {
	z = normalize(direction);
	x = cross(normalize(z), up);
	y = cross(normalize(z), x);
}
bool isFloatNearOther(Float a, Float b, Float error) {
	return fabsf(a - b) < error;
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
Float min(Float a, Float b) {
	if (a < b) return a;
	else return b;
}