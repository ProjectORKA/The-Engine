#pragma once

#include "Math.hpp"
#include "Prototype.hpp"
#include "Random.hpp"

struct SquareNeighborNode
{
	Vec3 position;
	ULL  north = -1;
	ULL  south = -1;
	ULL  west  = -1;
	ULL  east  = -1;

	SquareNeighborNode(const Vec3 position)
	{
		this->position = position;
	}
};

struct SquareNeighborPrototype : Prototype
{
	const Bool                 renderLines  = true;
	const Bool                 renderArrows = false;
	const ULL                  numPoints    = 10;
	Vector<SquareNeighborNode> points;

	void calculateGrid()
	{
		for(auto & point : points) {
			processNodes(point);
		}
	}

	void processNodes(SquareNeighborNode& point) const
	{
		Float minDistanceNorth = std::numeric_limits<float>::max();
		Float minDistanceSouth = std::numeric_limits<float>::max();
		Float minDistanceEast  = std::numeric_limits<float>::max();
		Float minDistanceWest  = std::numeric_limits<float>::max();

		for(ULL i = 0; i < points.size(); i++)
		{
			//dont process empty or self
			if(i == -1 || points[i].position == point.position) continue;

			// check for direction
			Vec3        delta    = points[i].position - point.position;
			const Float maxValue = max(abs(delta.x), abs(delta.y));
			const Float distance = length(delta);

			if(delta.x == maxValue)
			{
				// east
				if(distance < minDistanceEast)
				{
					minDistanceEast = distance;
					point.east      = i;
				}
			}
			else if(delta.x == -maxValue)
			{
				// west
				if(distance < minDistanceWest)
				{
					minDistanceWest = distance;
					point.west      = i;
				}
			}
			else if(delta.y == maxValue)
			{
				// north
				if(distance < minDistanceNorth)
				{
					minDistanceNorth = distance;
					point.north      = i;
				}
			}
			else if(delta.y == -maxValue)
			{
				//south
				if(distance < minDistanceSouth)
				{
					minDistanceSouth = distance;
					point.south      = i;
				}
			}
			else logError("The following vector should have been square normalized: " + toString(delta));
		}
	}

	void addPoint()
	{
		SquareNeighborNode point(randomVec3Fast(-10, 10, -10, 10, 0, 0));

		//if(points.empty()) goto finalize;

		////connect to 0
		//if(points[0].position.x > point.position.x) point.north = 0;
		//else point.south = 0;

		////make comparison copy
		//ULLVec4 copy;
		//do
		//{
		//	copy = ULLVec4(point.north,point.south,point.east,point.west);
		//	const Vector<ULL>travelNodes = compoundNeighbors({point.north,point.south,point.east,point.west});
		//	for(ULL n : travelNodes) processNodes(point, travelNodes);

		//}while(copy != ULLVec4(point.north,point.south,point.east,point.west));

		//finalize:;
		points.push_back(point);

		calculateGrid();
	}

	void create() override
	{
		for(ULL i = 0; i < numPoints; i++) addPoint();
	}

	void render(Renderer& r, const Player& player) override
	{
		r.useShader("debug");

		Vector<Matrix> matrices;
		Vector<Vec3>   connections;
		for(const auto p : points)
		{
			matrices.push_back(matrixFromPositionAndSize(p.position, 0.01f));
			if(renderArrows)
			{
				if(p.east != -1) r.arrow(p.position, points[p.east].position);
				if(p.west != -1) r.arrow(p.position, points[p.west].position);
				if(p.north != -1) r.arrow(p.position, points[p.north].position);
				if(p.south != -1) r.arrow(p.position, points[p.south].position);
			}

			if(renderLines)
			{
				if(p.east != -1)
				{
					connections.push_back(p.position);
					connections.push_back(points[p.east].position);
				}
				if(p.south != -1)
				{
					connections.push_back(p.position);
					connections.push_back(points[p.south].position);
				}
			}
		}

		if(renderLines) r.lines(connections);

		r.renderMeshInstanced("sphere", matrices);
	}

	void action() override
	{
		addPoint();
	}
};
