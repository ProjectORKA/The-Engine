#include "ParallelizedConvexHullPrototype.hpp"
#include "Player.hpp"

Vector<ULL> getConvexHullPointIDs(Vector<Vec2>& points)
{
	ULLVec4 bounds = getBoundingBoxIds(points);
	ULLVec4 currentIdOfBounds = bounds;
	Vector<ULL> nextID(points.size());
	nextID[bounds.x] = bounds.z;
	nextID[bounds.z] = bounds.y;
	nextID[bounds.y] = bounds.w;
	nextID[bounds.w] = bounds.x;
	Bool notFinished = false;
	do
	{
		notFinished = false;
		ULLVec4 nextIdForBounds(maxULL);
		Vec4    highestValue = Vec4(0);
		for(ULL i = 0; i < points.size(); i++)
		{
			const Vec2 deltaN = normalize(points[i] - points[currentIdOfBounds.z]);
			const Vec2 deltaS = normalize(points[i] - points[currentIdOfBounds.w]);
			const Vec2 deltaE = normalize(points[i] - points[currentIdOfBounds.x]);
			const Vec2 deltaW = normalize(points[i] - points[currentIdOfBounds.y]);

			if(-deltaN.x > highestValue.z)
			{
				highestValue.z    = -deltaN.x;
				nextIdForBounds.z = i;
			}
			if(+deltaS.x > highestValue.w)
			{
				highestValue.w    = +deltaS.x;
				nextIdForBounds.w = i;
			}
			if(+deltaE.y > highestValue.x)
			{
				highestValue.x    = +deltaE.y;
				nextIdForBounds.x = i;
			}
			if(-deltaW.y > highestValue.y)
			{
				highestValue.y    = -deltaW.y;
				nextIdForBounds.y = i;
			}
		}

		if(nextIdForBounds.x != maxULL && nextIdForBounds.x != currentIdOfBounds.x && nextIdForBounds.x != bounds.z)
		{
			nextID[currentIdOfBounds.x] = nextIdForBounds.x;
			nextID[nextIdForBounds.x] = bounds.z;
			if(nextIdForBounds.x != bounds.z) currentIdOfBounds.x = nextIdForBounds.x;
			notFinished = true;
		}

		if(nextIdForBounds.y != maxULL && nextIdForBounds.y != currentIdOfBounds.y && nextIdForBounds.y != bounds.w)
		{
			nextID[currentIdOfBounds.y] = nextIdForBounds.y;
			nextID[nextIdForBounds.y] = bounds.w;
			if(nextIdForBounds.y != bounds.w) currentIdOfBounds.y = nextIdForBounds.y;
			notFinished = true;
		}

		if(nextIdForBounds.z != maxULL && nextIdForBounds.z != currentIdOfBounds.z && nextIdForBounds.z != bounds.y)
		{
			nextID[currentIdOfBounds.z] = nextIdForBounds.z;
			nextID[nextIdForBounds.z] = bounds.y;
			if(nextIdForBounds.z != bounds.y) currentIdOfBounds.z = nextIdForBounds.z;
			notFinished = true;
		}

		if(nextIdForBounds.w != maxULL && nextIdForBounds.w != currentIdOfBounds.w && nextIdForBounds.w != bounds.x)
		{
			nextID[currentIdOfBounds.w] = nextIdForBounds.w;
			nextID[nextIdForBounds.w] = bounds.x;
			if(nextIdForBounds.w != bounds.x) currentIdOfBounds.w = nextIdForBounds.w;
			notFinished = true;
		}
	}
	while(notFinished);

	Vector<ULL> ids;
	ULL         start   = bounds.x;
	ULL         current = start;
	do
	{
		ids.push_back(current);
		current = nextID[current];
	}
	while(current != start);

	return ids;
}

void ParallelizedConvexHullPrototype::action()
{
	convexHullIds = getConvexHullPointIDs(points);
}

void ParallelizedConvexHullPrototype::create()
{
	for(ULL i = 0; i < pointCount; i++) points.push_back(randomPointInCircleFast(extend));
	player.camera.setLocation(0, 0, 150);
	player.camera.setRotation(PI, 0, 0);
}

void ParallelizedConvexHullPrototype::render(Renderer& r, const Player& player)
{
	r.useShader("debug");
	if(renderSpheres) r.renderMeshInstanced("sphere", points);

	if(renderPointCloud) r.points(points);

	if(drawConnections)
	{
		if(!convexHullIds.empty())
		{
			Vector<Vec2> boundaryLine;
			for(const auto& b : convexHullIds) boundaryLine.push_back(points[b]);
			boundaryLine.push_back(points[convexHullIds[0]]);
			r.fill(1, 0, 0);
			r.useShader("color");
			r.line(boundaryLine);
		}
	}
}
