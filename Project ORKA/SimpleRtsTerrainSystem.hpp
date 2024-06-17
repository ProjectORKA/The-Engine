#pragma once
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsTerrainSystem : SimpleRtsSystem
{
	void render(Renderer& renderer) const override {}
	void create(const SimpleRtsSimulation& sim) override {}
	void update(const SimpleRtsSimulation& sim) override {}
	void destroy(const SimpleRtsSimulation& sim) override {}

	[[nodiscard]] static Float simpleRtsTerrainFunction(Vec2 position);
};
