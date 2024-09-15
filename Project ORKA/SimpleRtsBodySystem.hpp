#pragma once

#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBodySystem : SimpleRtsSystem
{
	Int          count  = 1000;
	Float        radius = 1.5f;
	Vec3Vector positions;

	void update(SimpleRtsSimulation& sim) override;
	void render(Renderer& renderer) const override;
	void spawnEntity(const SimpleRtsSimulation& sim);
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;
};
