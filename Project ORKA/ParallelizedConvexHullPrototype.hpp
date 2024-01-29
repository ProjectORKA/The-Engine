#pragma once

#include "Player.hpp"
#include "Prototype.hpp"
#include "Random.hpp"

Vector<ULL> getConvexHullPointIDs(Vector<Vec2>& points);

struct ParallelizedConvexHullPrototype final : Prototype
{
	DebugPlayer  player;
	Vector<Vec2> points;
	Vector<ULL>  convexHullIds;
	ULL          pointCount       = 10000;
	Float        extend           = 100.0f;
	Bool         renderSpheres    = false;
	Bool         renderPointCloud = true;
	Bool         drawConnections  = true;

	void action() override;
	void create() override;
	void render(Renderer& r, const Player& player) override;
};
