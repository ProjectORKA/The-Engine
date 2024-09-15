#pragma once

#include "Math.hpp"
#include "Prototype.hpp"
#include "Random.hpp"

struct AxisNeighborNode2
{
	Vec3 position;
	Int  north = -1;
	Int  south = -1;
	Int  west  = -1;
	Int  east  = -1;

	void update(Vector<AxisNeighborNode2>& points, Float delta)
	{
		Float avgDist = distance(points[0].position, points[points[0].north].position);

		Vec3 pos   = Vec3(0);
		Int  count = 0;
		if(north != -1)
		{
			pos += points[north].position;
			count++;
		}
		if(south != -1)
		{
			pos += points[south].position;
			count++;
		}
		if(west != -1)
		{
			pos += points[west].position;
			count++;
		}
		if(east != -1)
		{
			pos += points[east].position;
			count++;
		}
		if(count) pos /= count;

		position = lerp(position, pos, 0.1f * delta) * avgDist;
		//position = 
	}

	AxisNeighborNode2(const Vec3 position)
	{
		this->position = position;
	}
};

struct AxisNeighborPrototype2 : Prototype
{
	const Bool                renderLines  = true;
	const Bool                renderArrows = false;
	const Int                 numPoints    = 1000;
	Vector<AxisNeighborNode2> points;
	//Vector<ULL> north;
	//Vector<ULL> south;
	//Vector<ULL> west;
	//Vector<ULL> east;

	void addPoint()
	{
		AxisNeighborNode2 point(randomVec3Fast(-1, 1, -1, 1, 0, 0));
		if(points.empty())
		{
			points.push_back(point);
			return;
		}
		const Int newPointID   = points.size();
		Int       comparisonID = 0;

		// x axis
		// go all the way west
		while(points[comparisonID].west != -1) comparisonID = points[comparisonID].west;

		// go east until comparison greater
		while(points[comparisonID].position.x < point.position.x)
		{
			// if we can, step east
			if(points[comparisonID].east != -1)
			{
				comparisonID = points[comparisonID].east;
			}

			// if we cant, point is inserted on the east
			else
			{
				point.west                = comparisonID;
				points[comparisonID].east = newPointID;
				goto insertYaxis;
			}
		}

		// inject the point between current and last
		if(points[comparisonID].west != -1)
		{
			//if last is valid, insert
			point.west = points[comparisonID].west;
			point.east = comparisonID;

			points[point.east].west = newPointID;
			points[point.west].east = newPointID;
			goto insertYaxis;
		}
		else
		{
			// if last is not valid, point is most west
			point.east                = comparisonID;
			points[comparisonID].west = newPointID;
			goto insertYaxis;
		}

	insertYaxis:;

		// y axis
		// go all the way south
		while(points[comparisonID].south != -1) comparisonID = points[comparisonID].south;

		// go north until comparison greater
		while(points[comparisonID].position.y < point.position.y)
		{
			// if we can, step north
			if(points[comparisonID].north != -1)
			{
				comparisonID = points[comparisonID].north;
			}

			// if we cant, point is inserted on the north
			else
			{
				point.south                = comparisonID;
				points[comparisonID].north = newPointID;
				goto finalize;
			}
		}

		// inject the point between current and last
		if(points[comparisonID].south != -1)
		{
			//if last is valid, insert
			point.south = points[comparisonID].south;
			point.north = comparisonID;

			points[point.north].south = newPointID;
			points[point.south].north = newPointID;
			goto finalize;
		}
		else
		{
			// if last is not valid, point is most south
			point.north                = comparisonID;
			points[comparisonID].south = newPointID;
			goto finalize;
		}

	finalize:;
		points.push_back(point);
	}

	void create() override
	{
		for(Int i = 0; i < numPoints; i++) addPoint();
	}

	void action() override
	{
		addPoint();
	}

	void render(Renderer& r, const Player& player) override
	{
		r.useShader("debug");

		Vector<Matrix> matrices;
		Vec3Vector   connections;
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

		for(AxisNeighborNode2& p : points) p.update(points, r.deltaTime());
	}
};
