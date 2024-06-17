#include "KDTreePrototype.hpp"
#include "KDTreeImplementation.hpp"
#include "Player.hpp"

void KDTreePrototype::action()
{
	if(time.isPaused())
	{
		time.reset();
		time.update();
		time.unpause();
	}
}

void KDTreePrototype::create()
{
	auto* playerPtr = dynamic_cast<DebugPlayer*>(&player);
	playerPtr->camera.setPosition(0, -250, 0);
	playerPtr->speedExponent = 35;

	pointCloud.points.resize(pointCount);
	for(SizeT i = 0; i < pointCount; i++) pointCloud.points[i] = randomVec3Fast(-100, 100, -1, 1, 0, 10000);
	time.pause();
}

SizeT KDTreeBase::pointCount() const
{
	return points.size();
}

void KDTreePrototype2::action(Renderer& r)
{
	if(time.isPaused())
	{
		time.reset();
		time.update();
		time.unpause();
	}

	addPoint();
}

Vec3 KDTreeBase::getPoint(const SizeT id) const
{
	return points[id];
}

void KDTreePrototype2::create(Renderer& r, Player& player)
{
	auto* playerPtr = dynamic_cast<DebugPlayer*>(&player);
	playerPtr->camera.setPosition(0, -250, 0);
	playerPtr->speedExponent = 35;

	for(SizeT i = 0; i < pointCount; i++) addPoint();
	time.pause();
}

void KDTreePrototype::render(Renderer& r, const Player& player)
{
	r.useShader("debug");
	r.renderMeshInstanced("sphere", pointCloud.points);

	time.update();

	if(velocity.empty())
	{
		velocity.resize(pointCloud.points.size());
		for(Vec3& v : velocity) v = randomUnitVec3Fast();
	}

	for(SizeT i = 0; i < velocity.size(); i++) pointCloud.points[i] += velocity[i] * time.getDelta();

	const KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<Float, KDTreeBase>, KDTreeBase, 3> kdTree(3, pointCloud, {10});

	lines.clear();

	for(SizeT i = 0; i < pointCount; i++)
	{
		Vec3                            currentPoint    = pointCloud.points[i];
		SizeT                           connectionCount = 0;
		Float                           searchRadius    = 500.0f;
		Vector<ResultItem<UInt, Float>> searchResult;
		connectionCount = kdTree.radiusSearch(&currentPoint.x, searchRadius, searchResult);
		Vec3 pushForce  = Vec3(0);

		for(SizeT j = 1; j < connectionCount; j++)
		{
			Vec3        otherPoint = pointCloud.points[searchResult[j].first];
			const Float distance   = searchResult[j].second;
			pushForce += Vec3(10 / distance) * normalize(currentPoint - otherPoint);
			pushForce = clamp(pushForce, Vec3(-100000), Vec3(100000));

			lines.push_back(currentPoint);
			lines.push_back(otherPoint);
		}
		velocity[i] += Vec3(20.0f) * pushForce - Vec3(150) * normalize(pointCloud.points[i]) * time.getDelta();
		approach(velocity[i], Vec3(0), time.getDelta() * 1.f);
	}

	r.lines(lines);
}

Float KDTreeBase::getPoint(const SizeT id, const SizeT axis) const
{
	if(axis == 0) return points[id].x;
	if(axis == 1) return points[id].y;
	return points[id].z;
}
