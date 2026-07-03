#pragma once
#include "Prototype.hpp"
#include "Random.hpp"

inline Float distanceSq(const Vec2& a, const Vec2& b)
{
	const Float dx = a.x - b.x;
	const Float dy = a.y - b.y;
	return dx * dx + dy * dy;
}

struct HeuristicNetworkingPrototype final : Prototype
{
	Vec2Vector    points;
	Vector<Float> distances;
	Vector<Int>   connections;
	Vector<Float> furthestNeighborDistance;

	const Int numSlots = 4;

	Int pointCount = 100;

	void action() override
	{
		//cycle through all points eventually
		static Int randomPointId = -1;
		randomPointId++;
		if (randomPointId >= pointCount) randomPointId = 0;
		//for (Int randomPointId = 0; randomPointId < pointCount; randomPointId++)
		for (Int pointId = 0; pointId < pointCount; pointId++)
		{
			//ignore self
			if (pointId == randomPointId) continue;

			//avoid duplicates
			Bool isDuplicate = false;
			for (Int slot = 0; slot < numSlots; slot++)
			{
				if (connections[pointId * numSlots + slot] == randomPointId)
				{
					isDuplicate = true;
					break;
				}
			}
			if (isDuplicate) continue;

			//get distance
			const Float dist = distanceSq(points[randomPointId], points[pointId]);

			//if not closer ignore
			if (dist >= furthestNeighborDistance[pointId]) continue;

			Float newFurthestNeighborDistance = 0.f;

			Bool canInsert = true;
			//cycle through slots
			for (Int slot = 0; slot < numSlots; slot++)
			{
				const Int connectionId = pointId * numSlots + slot;
				if (canInsert && distances[connectionId] == furthestNeighborDistance[pointId])
				{
					//replace slot
					connections[connectionId] = randomPointId;
					distances[connectionId] = dist;
					canInsert = false;
				}
				newFurthestNeighborDistance = max(distances[connectionId],newFurthestNeighborDistance);
			}

			//set new furthest distance
			furthestNeighborDistance[pointId] = newFurthestNeighborDistance;
		}
	}

	void destroy() override {}

	void create(Renderer& r) override
	{
		for (Int i = 0; i < pointCount; i++)
		{
			points.emplace_back(randomVec2Fast(-5, 5));
			furthestNeighborDistance.emplace_back(std::numeric_limits<float>::max());
		}
		for (Int i = 0; i < pointCount * numSlots; i++)
		{
			connections.emplace_back(-1);
			distances.emplace_back(std::numeric_limits<float>::max());
		}
	}

	void render(Renderer& r, const Player& player) override
	{
		r.uniforms().setMMatrix(1);
		r.points(points);
		Vec2Vector lines;

		for (Int i = 0; i < pointCount * numSlots; i++)
		{
			const Int id = connections[i];
			if (id < 0) continue;
			lines.emplace_back(points[i/numSlots]);
			lines.emplace_back(points[id]);
		}
		logDebug(lines.size() / 2);
		logDebug(pointCount * numSlots);
		r.lines(lines);
	}

	void inputEvent(Window& window, InputEvent& input) override {}
};
