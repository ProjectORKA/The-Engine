#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBerryBushSystem : SimpleRtsEntitySystem
{
	[[nodiscard]] Name  getEntityName() const override;
	[[nodiscard]] Int   getEntityCount() const override;
	[[nodiscard]] Float getEntityRadius() const override;

	void update(SimpleRtsSimulation& sim) override {}
	void eat(Index berryBushId, SimpleRtsSimulation& sim);
	void destroy(const SimpleRtsSimulation& sim) override {};
	void spawnEntity(const SimpleRtsSimulation& sim) override;
};
