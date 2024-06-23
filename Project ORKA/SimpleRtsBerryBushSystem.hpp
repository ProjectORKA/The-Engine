#pragma once
#include "Basics.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBerryBushSystem : SimpleRtsSystem
{
	Int          count  = 5000;
	Float        radius = 1.0f;
	Vector<Vec2> positions;

	void update(SimpleRtsSimulation& sim) override {}
	void eat(Index berryBushId, SimpleRtsSimulation& sim);
	void destroy(const SimpleRtsSimulation& sim) override;
	void spawnEntity(const SimpleRtsSimulation& sim);

	void render(Renderer& renderer) const override;

	void create(const SimpleRtsSimulation& sim) override;
};
