#include "Math.hpp"
#include "TiledMath.hpp"

void Graph::add(const Float value) { points.push_back(value); }

void loopWithinCentered(Vec2& point, const Float extend) {
	while (point.x < -extend) point.x += extend * 2;
	while (point.x >= extend) point.x -= extend * 2;
	while (point.y < -extend) point.y += extend * 2;
	while (point.y >= extend) point.y -= extend * 2;
}

void removePointsInRadius(const Vec3 point, Vector<Vec3>& points, const Float radius) {
	Vector<Vec3> available;

	for (Vec3 current : points) { if (distance(point, current) > radius) { available.push_back(current); } }

	points = available;
}

void getClosestPoint(const Vec3 point, const Vector<Vec3>& points, Index& closestId, Vec3& closestPoint) {
	Float minimalDistance = distance(point, points[0]);
	Index closestPointId = 0;

	for (Int i = 0; i < points.size(); i++) {
		const Float currentDistance = distance(point, points[i]);
		if (currentDistance < minimalDistance) {
			minimalDistance = currentDistance;
			closestPointId = i;
		}
	}

	closestId = closestPointId;
	closestPoint = points[closestPointId];
}

void spaceColonization(Vector<Vec3>& points, Vector<Vec3>& branches, Vector<Index>& connections,
                       const Float segmentSize, const Float killRadius) {
	if (points.empty()) return;
	if (branches.empty()) branches.push_back(Vec3(0));
	if (connections.empty()) connections.push_back(0);

	Vector<Vec3> pull(branches.size(), Vec3(0));

	//check every space
	for (const Vec3& point : points) {
		Index closestId;
		Vec3 closestPoint;
		getClosestPoint(point, branches, closestId, closestPoint);
		//add pull force to branches
		pull[closestId] += vectorFromAToB(closestPoint, point);
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
	for (const Vec3 node : newBranches) { removePointsInRadius(node, points, killRadius); }

	//add branches
	for (Vec3 node : newBranches) { branches.push_back(node); }
}

Bool isOdd(const ULL a) { return a % 2 != 0; }
Bool isEven(const ULL a) { return a % 2 == 0; }
Bool withinLength(const Vec3 a, const Float b) { return (a.x * a.x + a.y * a.y + a.z * a.z) < (b * b); };
Bool withinDiamondArea(const Vec3 a, const Float b) { return (abs(a.x) + abs(a.y) + abs(a.z)) < 2 * b; };

Bool isNear(const Vec3 a, const Vec3 b, const Float error) {
	return isNear(a.x, b.x, error) && isNear(a.y, b.y, error) && isNear(a.z, b.z, error);
}

Bool isNear(const Float a, const Float b, const Float error) { return fabsf(a - b) < error; }

Bool pointInsideSphere(const Vec3 point, const Sphere sphere) {
	return pointInsideSphereAtlocationWithRadius(point, Vec3(sphere), sphere.w);
}

Bool pointInsideSpheres(const Vec3 point, List<Sphere> spheres) {
	auto it = spheres.begin();

	while (it != spheres.end()) {
		if (pointInsideSphere(point, *it)) return true;
		++it;
	}
	return false;
}

Bool pointInsideSpheres(const Vec3 point, const Vector<Sphere>& spheres) {
	for (const auto s : spheres) { if (pointInsideSphere(point, s)) return true; }
	return false;
}

Bool isWithinDistanceOfOtherPoints(const Vec2 point, Vector<Vec2>& points, const Float dist) {
	for (const Vec2& p : points) { if (distance(point, p) < dist) return true; }
	return false;
}

Bool pointInsideSphereAtlocationWithRadius(const Vec3 point, const Vec3 position, const Float radius) {
	return distance(point, position) < radius;
}

UInt max(const UInt a, const UInt b) {
	if (a > b) return a;
	return b;
}

UInt nextPowerOfTwo(const UInt& value) {
	UInt powerOfTwo = 1;
	while (powerOfTwo < value) powerOfTwo <<= 1;
	return powerOfTwo;
}

UInt countBitsInFlags(Short flags) {
	UInt count = 0;
	while (flags) {
		count++;
		flags = flags >> 1;
	}
	return count;
}

UInt fibonacciSequence(const UInt iterations) {
	Bool isX = true;
	UInt x = 1;
	UInt y = 1;

	for (UInt i = 0; i < iterations - 3; i++) {
		if (isX) x += y;
		else y += x;
		isX = !isX;
	}

	if (isX) return x;
	return y;
}

Index idOfClosestPoint(const Vec2 origin, const Vector<Vec2>& positions) {
	Index index = 0;
	Float closestDistance = distance(origin, positions.front());
	for (Index i = 1; i < positions.size(); i++) {
		const Float currentDistance = distance(origin, positions[i]);
		if (currentDistance < closestDistance) {
			closestDistance = currentDistance;
			index = i;
		}
	}
	return index;
}

Index idOfClosestPointInLoopingSpace(const Vec2 origin, const Vector<Vec2>& positions, const Float extend) {
	Index index = 0;
	Float closestDistance = distanceToPointInLoopingSpace(origin, positions.front(), extend);
	for (Index i = 1; i < positions.size(); i++) {
		const Float currentDistance = distanceToPointInLoopingSpace(origin, positions[i], extend);
		if (currentDistance < closestDistance) {
			closestDistance = currentDistance;
			index = i;
		}
	}
	return index;
}

Int max(const Int a, const Int b) {
	if (a > b) return a;
	return b;
}

Float sq(const Float a) { return a * a; }

Float min(const Float a, const Float b) {
	if (a < b) return a;
	return b;
}

Float mod(const Float a, const Float b) { return fmod(a, b); }
Float snap(const Float a, const Float b) { return round(a / b) * b; }
Float distance(const Vec2 a, const Vec2 b) { return glm::distance(a, b); }
Float distance(const Float a, const Float b) { return abs(a - b); }
Float approach(const Float input, const Float maxValue) { return (maxValue * input) / (maxValue + abs(input)); }

Float lerp(const Float a, Float b, const Float alpha) {
	if (isnan(b)) b = 0;
	return (a * (1 - alpha)) + (b * alpha);
}

Float clerp(const Float a, const Float b, Float alpha) {
	alpha = clamp(alpha, 0, 1);
	return lerp(a, b, alpha);
}

Float deltaInLoopingSpace(const Float a, const Float b, const Float extend) {
	const Float delta1 = b - a;
	Float delta2;
	if (a < 0) { delta2 = b - (extend * 2) - a; }
	else { delta2 = b + (extend * 2) - a; }

	if (abs(delta1) < abs(delta2)) return delta1;
	return delta2;
}

Float distanceToPointInLoopingSpace(const Vec2 a, const Vec2 b, const Float extend) {
	return length(deltaInLoopingSpace(a, b, extend));
}

Float getDistanceToClosestPoint(const Vec3 point, const Vector<Vec3>& points) {
	Float minimalDistance = distance(point, points[0]);

	for (auto p : points) { minimalDistance = min(minimalDistance, distance(point, p)); }

	return minimalDistance;
}

LDouble dmod(const LDouble x, const LDouble y) {
	return x - static_cast<ULL>(x / y) * y; //[TODO] check if it actually works
}

LDouble lerp(const LDouble a, const LDouble b, const LDouble alpha) { return (a * (1 - alpha)) + (b * alpha); }

LDouble clerp(const LDouble a, const LDouble b, LDouble alpha) {
	alpha = clamp(alpha, 0, 1);
	return lerp(a, b, alpha);
}

Vec2 vectorFromAToB(const Vec2 a, const Vec2 b) { return b - a; }

Vec2 clerp(const Vec2 a, const Vec2 b, const Float alpha) {
	Vec2 c;
	c.x = clerp(a.x, b.x, alpha);
	c.y = clerp(a.y, b.y, alpha);
	return c;
}

Vec2 deltaInLoopingSpace(const Vec2 a, const Vec2 b, const Float extend) {
	return Vec2(deltaInLoopingSpace(a.x, b.x, extend), deltaInLoopingSpace(a.y, b.y, extend));
}

Vec3 max(const Vec3 a, const Vec3 b) { return Vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }

Vec3 projectToCube(const Vec3 vec) {
	Float max = abs(vec.x);
	if (abs(vec.y) > max) max = abs(vec.y);
	if (abs(vec.z) > max) max = abs(vec.z);
	return vec / Vec3(max);
}

Vec3 vectorFromAToB(const Vec3 a, const Vec3 b) { return b - a; }

Vec3 lerp(const Vec3 a, const Vec3 b, const Float alpha) {
	Vec3 c;
	c.x = lerp(a.x, b.x, alpha);
	c.y = lerp(a.y, b.y, alpha);
	c.z = lerp(a.z, b.z, alpha);
	return c;
}

Vec3 clerp(const Vec3 a, const Vec3 b, const Float alpha) {
	Vec3 c;
	c.x = clerp(a.x, b.x, alpha);
	c.y = clerp(a.y, b.y, alpha);
	c.z = clerp(a.z, b.z, alpha);
	return c;
}

Vec3 getClosestPoint(const Vec3 point, const List<Vec3>& points) {
	if (points.empty()) return point;

	Float minimalDistance = distance(point, points.front());
	Vec3 closestPointTMP = points.front();

	for (auto p : points) {
		const Float currentDistance = distance(point, p);
		if (currentDistance < minimalDistance) {
			minimalDistance = currentDistance;
			closestPointTMP = p;
		}
	}

	return closestPointTMP;
}

Vec3 quadraticInterpolation(const Vec3 start, const Vec3 control, const Vec3 end, const Float time) {
	return lerp(
		lerp(start, control, time),
		lerp(control, end, time),
		time
	);
}

Rotation getRotationBetweenVectors(Vec3 start, Vec3 dest) {
	//copy of opengl tutorial
	start = normalize(start);
	dest = normalize(dest);

	const float cosTheta = dot(start, dest);
	Vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = cross(Vec3(0.0f, 0.0f, 1.0f), start);
		if (length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = cross(Vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = cross(start, dest);

	const float s = sqrt((1 + cosTheta) * 2);
	const float invs = 1 / s;

	return Rotation(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

Orientation::Orientation(const Vec3 direction) {
	//this function just assumes +Z is up
	z = normalize(direction);
	x = cross(normalize(z), Vec3(0, 0, 1));
	y = cross(normalize(z), x);
}

Orientation::Orientation(const Vec3 direction, const Vec3 up) {
	z = normalize(direction);
	x = cross(normalize(z), up);
	y = cross(normalize(z), x);
}

Matrix clerp(Matrix a, Matrix b, Float alpha) {
	alpha = clamp(alpha, 0, 1);

	const auto m = Matrix(
		lerp(a[0].x, b[0].x, alpha), lerp(a[0].y, b[0].y, alpha), lerp(a[0].z, b[0].z, alpha),
		lerp(a[0].w, b[0].w, alpha),
		lerp(a[1].x, b[1].x, alpha), lerp(a[1].y, b[1].y, alpha), lerp(a[1].z, b[1].z, alpha),
		lerp(a[1].w, b[1].w, alpha),
		lerp(a[2].x, b[2].x, alpha), lerp(a[2].y, b[2].y, alpha), lerp(a[2].z, b[2].z, alpha),
		lerp(a[2].w, b[2].w, alpha),
		lerp(a[3].x, b[3].x, alpha), lerp(a[3].y, b[3].y, alpha), lerp(a[3].z, b[3].z, alpha),
		lerp(a[3].w, b[3].w, alpha)
	);

	return m;
}

Matrix matrixFromScale(const Vec2 s) {
	Matrix m;
	m[0] = Vec4(s.x, 0, 0, 0);
	m[1] = Vec4(0, s.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}

Matrix matrixFromScale(const Vec3 s) {
	Matrix m;
	m[0] = Vec4(s.x, 0, 0, 0);
	m[1] = Vec4(0, s.y, 0, 0);
	m[2] = Vec4(0, 0, s.z, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}

Matrix matrixFromScale(const Float size) {
	Matrix m;
	m[0] = Vec4(size, 0, 0, 0);
	m[1] = Vec4(0, size, 0, 0);
	m[2] = Vec4(0, 0, size, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}

Matrix matrixFromLocation(const Vec2 location) {
	Matrix m(1);
	m[3].x = location.x;
	m[3].y = location.y;
	return m;
}

Matrix matrixFromLocation(const Vec3 location) {
	Matrix m(1);
	m[3] = Vec4(location, 1);
	return m;
}

Matrix matrixFromOrientation(const Orientation& o) { return matrixFromAxis(o.x, o.y, o.z); }

Matrix matrixFromAxis(const Vec3 x, const Vec3 y, const Vec3 z) {
	Matrix m;
	m[0] = Vec4(x, 0);
	m[1] = Vec4(y, 0);
	m[2] = Vec4(z, 0);
	m[3] = Vec4(0, 0, 0, 1);
	return m;
}

Matrix matrixFromTiledRectangle(const TiledRectangle area) {
	Matrix m;
	m[0] = Vec4(area.size.x, 0, 0, 0);
	m[1] = Vec4(0, area.size.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(area.position.x, area.position.y, 0, 1);
	return m;
}

Matrix matrixFromRotation(const Float x, const Float y, const Float z) { return glm::eulerAngleYXZ(z, x, y); }

Matrix matrixFromLocationAndSize2D(const Vec2 pos, const Vec2 size) {
	Matrix m(1);
	m[0] = Vec4(size.x, 0, 0, 0);
	m[1] = Vec4(0, size.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
}

Matrix matrixFromPositionAndDirection(const Vec2 pos, const Vec2 dir) {
	Matrix m(1);
	m[0] = Vec4(dir.y, -dir.x, 0, 0);
	m[1] = Vec4(dir.x, dir.y, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
}

Matrix matrixFromLocationAndSize(const Vec4 compressedTransform) {
	Matrix m(compressedTransform.w);
	m[3] = compressedTransform;
	m[3].w = 1;
	return m;
}

Matrix matrixFromLocationAndSize(const Vec3 location, const Float size) {
	Matrix m(size);
	m[3] = Vec4(location, 1);
	return m;
}

Matrix matrixFromLocationAndSize(const Vec2 location, const Float size) {
	Matrix m(size);
	m[3] = Vec4(location, 0, 1);
	return m;
}

Matrix matrixFromLocationAndSize2D(const Float x, const Float y, const Float w, const Float h) {
	Matrix m(1);
	m[0] = Vec4(w, 0, 0, 0);
	m[1] = Vec4(0, h, 0, 0);
	m[2] = Vec4(0, 0, 1, 0);
	m[3] = Vec4(x, y, 0, 1);
	return m;
}

Matrix matrixFromOrientation(const Orientation& o, const Vec3 position, const Float size) {
	return matrixFromAxis(o.x, o.y, o.z, position, size);
}

Matrix matrixFromAxis(const Vec3 x, const Vec3 y, const Vec3 z, const Vec3 position, const Float size) {
	Matrix m;
	m[0] = Vec4(size * x, 0);
	m[1] = Vec4(size * y, 0);
	m[2] = Vec4(size * z, 0);
	m[3] = Vec4(position, 1);
	return m;
}

Matrix matrixFromLocationDirectionAndSize(const Vec2 pos, Vec2 dir, const Float size) {
	Matrix m(1);
	dir *= size;
	m[0] = Vec4(-dir.y, dir.x, 0, 0);
	m[1] = Vec4(-dir.x, -dir.y, 0, 0);
	m[2] = Vec4(0, 0, size, 0);
	m[3] = Vec4(pos.x, pos.y, 0, 1);
	return m;
}

Matrix screenSpaceMatrix(const Float width, const Float height) {
	auto matrix = Matrix(1);
	matrix = translate(matrix, Vec3(-1, -1, 0));
	matrix = scale(matrix, Vec3(2.0 / width, 2.0 / height, 1));
	return matrix;
}
