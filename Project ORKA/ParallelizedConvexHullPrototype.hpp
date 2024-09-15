#pragma once

#include "Player.hpp"
#include "Prototype.hpp"
#include "Random.hpp"

Vector<ULL> getConvexHullPointIDs(Vec2Vector& points);

struct ParallelizedConvexHullPrototype final : Prototype
{
	DebugPlayer  player;
	Vec2Vector points;
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
