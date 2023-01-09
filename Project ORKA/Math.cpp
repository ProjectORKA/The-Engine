
#include "Math.hpp"
#include "TiledMath.hpp"

void Graph::add(Float value) {
	points.push_back(value);
}

Bool isOdd(ULL a) {
	return a % 2 != 0;
}
Bool isEven(ULL a) {
	return a % 2 == 0;
}
Bool withinLength(Vec3 a, Float b) {
	return (a.x * a.x + a.y * a.y + a.z * a.z) < (b * b);
};
Bool withinDiamondArea(Vec3 a, Float b) {
	return (abs(a.x) + abs(a.y) + abs(a.z)) < 2 * b;
};
Bool isNear(Float a, Float b, Float error) {
	return fabsf(a - b) < error;
}
Bool pointInsideSphere(Vec3 point, Sphere sphere) {
	return pointInsideSphereAtlocationWithRadius(point, Vec3(sphere), sphere.w);
}
Bool pointInsideSpheres(Vec3 point, List<Sphere> spheres) {
	auto it = spheres.begin();

	while (it != spheres.end()) {
		if (pointInsideSphere(point, *it)) return 1;
		it++;
	}
	return 0;
}
Bool pointInsideSpheres(Vec3 point, Vector<Sphere> spheres) {
	for (auto s : spheres) {
		if (pointInsideSphere(point, s)) return 1;
	}
	return 0;
}
Bool isWithinDistanceOfOtherPoints(Vec2 point, Vector<Vec2>& points, Float dist) {
	for (Vec2& p : points) {
		if (distance(point, p) < dist) return true;
	}
	return false;
}
Bool pointInsideSphereAtlocationWithRadius(Vec3 point, Vec3 position, Float radius) {
	return distance(point, position) < radius;
}

Bool isNear(Vec3 a, Vec3 b, Float error) {
	return isNear(a.x, b.x, error) && isNear(a.y, b.y, error) && isNear(a.z, b.z, error);
}

UInt max(UInt a, UInt b) {
	if (a > b) return a;
	else return b;
}
UInt nextPowerOfTwo(UInt& value)
{
	UInt powerOfTwo = 1;
	while (powerOfTwo < value) powerOfTwo <<= 1;
	return powerOfTwo;
}
UInt fibonacciSequence(UInt iterations) {
	Bool isX = true;
	UInt x = 1;
	UInt y = 1;

	for (UInt i = 0; i < iterations - 3; i++) {
		if (isX) x += y;
		else y += x;
		isX = !isX;
	}

	if (isX) return x;
	else return y;
}

UInt countBitsInFlags(Short flags) {
	UInt count = 0;
	while (flags) {
		count++;
		flags = flags >> 1;
	}
	return count;
}

Index idOfClosestPoint(Vec2 origin, Vector<Vec2>& positions) {
	Index index = 0;
	Float closestDistance = distance(origin, positions.front());
	for (Index i = 1; i < positions.size(); i++) {
		Float currentDistance = distance(origin, positions[i]);
		if (currentDistance < closestDistance) {
			closestDistance = currentDistance;
			index = i;
		}
	}
	return index;
}
Index idOfClosestPointInLoopingSpace(Vec2 origin, Vector<Vec2>& positions, Float extend) {
	Index index = 0;
	Float closestDistance = distanceToPointInLoopingSpace(origin, positions.front(),extend);
	for (Index i = 1; i < positions.size(); i++) {
		Float currentDistance = distanceToPointInLoopingSpace(origin, positions[i], extend);
		if (currentDistance < closestDistance) {
			closestDistance = currentDistance;
			index = i;
		}
	}
	return index;
}

Int max(Int a, Int b) {
	if (a > b) return a;
	else return b;
}

Float sq(Float a) {
	return a * a;
}
Float min(Float a, Float b) {
	if (a < b) return a;
	else return b;
}
Float mod(Float a, Float b) {
	return fmod(a, b);
}
Float snap(Float a, Float b) {
	return round(a / b) * b;
}
Float distance(Vec2 a, Vec2 b) {
	return glm::distance(a, b);
}
Float distance(Float a, Float b) {
	return abs(a - b);
}
Float approach(Float input, Float maxValue) {
	return (maxValue * input) / (maxValue + abs(input));
}
Float lerp(Float a, Float b, Float alpha)
{
	return (a * (1 - alpha)) + (b * alpha);
}
Float clerp(Float a, Float b, Float alpha)
{
	alpha = clamp(alpha, 0, 1);
	return lerp(a,b,alpha);
}
Float deltaInLoopingSpace(Float a, Float b, Float extend) {
	Float delta1 = b - a;
	Float delta2;
	if (a < 0) {
		delta2 = b - (extend * 2) - a;
	}
	else {
		delta2 = b + (extend * 2) - a;
	}

	if (abs(delta1) < abs(delta2)) return delta1;
	else return delta2;
}
Float getDistanceToClosestPoint(Vec3 point, Vector<Vec3>& points) {
	Float minimalDistance = distance(point, points[0]);

	for (auto p : points) {
		minimalDistance = min(minimalDistance, distance(point, p));
	}

	return minimalDistance;
}
Float distanceToPointInLoopingSpace(Vec2 a, Vec2 b, Float extend) {
	return length(deltaInLoopingSpace(a, b, extend));
}

LDouble dmod(LDouble x, LDouble y) {
	return x - (ULL)(x / y) * y; //[TODO] check if it actually works
}
LDouble lerp(LDouble a, LDouble b, LDouble alpha)
{
	return (a * (1 - alpha)) + (b * alpha);
}
LDouble clerp(LDouble a, LDouble b, LDouble alpha)
{
	alpha = clamp(alpha, 0, 1);
	return lerp(a,b,alpha);
}

Vec2 vectorFromAToB(Vec2 a, Vec2 b) {
	return b - a;
}
Vec2 clerp(Vec2 a, Vec2 b, Float alpha) {
	Vec2 c;
	c.x = clerp(a.x, b.x, alpha);
	c.y = clerp(a.y, b.y, alpha);
	return c;
}
Vec2 deltaInLoopingSpace(Vec2 a, Vec2 b, Float extend) {
	return Vec2(deltaInLoopingSpace(a.x, b.x, extend), deltaInLoopingSpace(a.y, b.y, extend));
}

Vec3 max(Vec3 a, Vec3 b) {
	return Vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}
Vec3 projectToCube(Vec3 vec) {
	Float max = abs(vec.x);
	if (abs(vec.y) > max) max = abs(vec.y);
	if (abs(vec.z) > max) max = abs(vec.z);
	return vec / Vec3(max);
}
Vec3 vectorFromAToB(Vec3 a, Vec3 b) {
	return b - a;
}
Vec3 lerp(Vec3 a, Vec3 b, Float alpha) {
	Vec3 c;
	c.x = lerp(a.x, b.x, alpha);
	c.y = lerp(a.y, b.y, alpha);
	c.z = lerp(a.z, b.z, alpha);
	return c;
}
Vec3 clerp(Vec3 a, Vec3 b, Float alpha) {
	Vec3 c;
	c.x = clerp(a.x, b.x, alpha);
	c.y = clerp(a.y, b.y, alpha);
	c.z = clerp(a.z, b.z, alpha);
	return c;
}
Vec3 getClosestPoint(Vec3 point, List<Vec3>& points) {

	if (points.size() == 0) return point;

	Float minimalDistance = distance(point, points.front());
	Vec3 closestPointTMP = points.front();

	for (auto p : points) {
		Float currentDistance = distance(point, p);
		if (currentDistance < minimalDistance) {
			minimalDistance = currentDistance;
			closestPointTMP = p;
		}
	}

	return closestPointTMP;
}
Vec3 quadraticInterpolation(Vec3 start, Vec3 control, Vec3 end, Float time) {
	return lerp(
		lerp(start, control, time),
		lerp(control, end, time),
		time
	);
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

Orientation::Orientation(Vec3 direction) {
	//this function just assumes +Z is up
	z = normalize(direction);
	x = cross(normalize(z), Vec3(0, 0, 1));
	y = cross(normalize(z), x);
}
Orientation::Orientation(Vec3 direction, Vec3 up) {
	z = normalize(direction);
	x = cross(normalize(z), up);
	y = cross(normalize(z), x);
}

Matrix matrixFromScale(Vec2 s) {
	Matrix m;
	m[0] = Vec4(s.x, 0, 0, 0);
	m[1] = Vec4(0, s.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}
Matrix matrixFromScale(Vec3 s) {
	Matrix m;
	m[0] = Vec4(s.x, 0, 0, 0);
	m[1] = Vec4(0, s.y, 0, 0);
	m[2] = Vec4(0, 0, s.z, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}
Matrix matrixFromScale(Float size) {
	Matrix m;
	m[0] = Vec4(size, 0, 0, 0);
	m[1] = Vec4(0, size, 0, 0);
	m[2] = Vec4(0, 0, size, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}
Matrix matrixFromLocation(Vec2 location) {
	Matrix m(1);
	m[3].x = location.x;
	m[3].y = location.y;
	return m;
}
Matrix matrixFromLocation(Vec3 location) {
	Matrix m(1);
	m[3] = Vec4(location, 1);
	return m;
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
Matrix matrixFromTiledRectangle(TiledRectangle area) {
	Matrix m;
	m[0] = Vec4(area.size.x, 0, 0, 0);
	m[1] = Vec4(0, area.size.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(area.position.x, area.position.y, 0, 1);
	return m;
}
Matrix matrixFromRotation(Float x, Float y, Float z) {
	return	glm::eulerAngleYXZ(z, x, y);
}
Matrix matrixFromLocationAndSize2D(Vec2 pos, Vec2 size) {
	Matrix m(1);
	m[0] = Vec4(size.x, 0, 0, 0);
	m[1] = Vec4(0, size.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
}
Matrix matrixFromPositionAndDirection(Vec2 pos, Vec2 dir) {
	Matrix m(1);
	m[0] = Vec4(dir.y, -dir.x, 0, 0);
	m[1] = Vec4(dir.x, dir.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
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
Matrix matrixFromLocationAndSize(Vec2 location, Float size)
{
	Matrix m(size);
	m[3] = Vec4(location, 0, 1);
	return m;
}
Matrix matrixFromLocationAndSize2D(Float x, Float y, Float w, Float h) {
	Matrix m(1);
	m[0] = Vec4(w, 0, 0, 0);
	m[1] = Vec4(0, h, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(x, y, 0, 1);
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
Matrix matrixFromLocationDirectionAndSize(Vec2 pos, Vec2 dir, Float size) {
	Matrix m(1);
	dir *= size;
	m[0] = Vec4(-dir.y, dir.x, 0, 0);
	m[1] = Vec4(-dir.x, -dir.y, 0, 0);
	m[2] = Vec4(0, 0, size, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
}

void loopWithin(Vec2& point, Float extend) {
	while (point.x < -extend) point.x += extend * 2;
	while (point.x > +extend) point.x -= extend * 2;
	while (point.y < -extend) point.y += extend * 2;
	while (point.y > +extend) point.y -= extend * 2;
}
void removePointsInRadius(Vec3 point, Vector<Vec3>& points, Float radius) {
	Vector<Vec3> available;

	for (Vec3 current : points) {
		if (distance(point, current) > radius) {
			available.push_back(current);
		}
	}

	points = available;
}
void getClosestPoint(Vec3 point, Vector<Vec3>& points, Index& closestID, Vec3& closestPoint) {

	Float minimalDistance = distance(point, points[0]);
	Index closestPointID = 0;

	for (Int i = 0; i < points.size(); i++) {
		Float currentDistance = distance(point, points[i]);
		if (currentDistance < minimalDistance) {
			minimalDistance = currentDistance;
			closestPointID = i;
		}
	}

	closestID = closestPointID;
	closestPoint = points[closestPointID];
}
void spaceColonization(Vector<Vec3>& points, Vector<Vec3>& branches, Vector<Index>& connections, Float segmentSize, Float killRadius) {
	if (points.size() == 0) return;
	if (branches.size() == 0) branches.push_back(Vec3(0));
	if (connections.size() == 0) connections.push_back(0);

	Vector<Vec3> pull(branches.size(), Vec3(0));

	//check every space
	for (Vec3& point : points) {
		Index closestID;
		Vec3 closestPoint;
		getClosestPoint(point, branches, closestID, closestPoint);
		//add pull force to branches
		pull[closestID] += vectorFromAToB(closestPoint, point);
	}

	Vector<Vec3> newBranches;

	//calculate branches
	for (int i = 0; i < pull.size(); i++) {
		if (pull[i] != Vec3(0)) {
			//calculate direction
			pull[i] = normalize(pull[i]);
			//add branch
			Vec3 newBranchSegment = branches[i] + segmentSize * pull[i];
			newBranches.push_back(newBranchSegment);
			connections.push_back(i);
		}
	}

	//kill points
	for (Vec3 node : newBranches) {
		removePointsInRadius(node, points, killRadius);
	}

	//add branches
	for (Vec3 node : newBranches) {
		branches.push_back(node);
	}
}