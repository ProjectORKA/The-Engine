#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBerryBushSystem : SimpleRtsSystem
{
	Float      radius = 0.5f;
	Vec3Vector positions;
	Mutex mutex;

	void update(SimpleRtsSimulation& sim) override;
	void eat(Index berryBushId, SimpleRtsSimulation& sim);
	void destroy(const SimpleRtsSimulation& sim) override;
	void spawnEntity(const SimpleRtsSimulation& sim);
	void render(Renderer& renderer) override;
	void create(const SimpleRtsSimulation& sim) override;

	Bool doesCollide(const Vec2 position, const Float radius) const;
};
