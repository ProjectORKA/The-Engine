#pragma once
#include "Prototype.hpp"
#include "Random.hpp"

struct ClosestConnectionPrototype : Prototype
{
	const int    pointCount = 10000;
	Vector<Vec2> points;
	Vector<Vec2> lines;

	void addPoint()
	{
		const Vec2 newPoint     = randomVec2Fast(-100.0, 100.0);
		const Vec2 closestPoint = getClosestPoint(newPoint, points);
		points.push_back(newPoint);
		lines.push_back(newPoint);
		lines.push_back(closestPoint);
	}

	void action() override {}

	void create() override {}

	void render(Renderer& r, const Player& player) override
	{
		if (points.size() < pointCount) addPoint();
		r.lines(lines);
		r.useShader("debug");
		r.points(points);
	}

	void destroy() override {}
};
