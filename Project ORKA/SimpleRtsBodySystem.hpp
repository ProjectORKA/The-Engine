#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBodySystem : SimpleRtsSystem
{
	Vec3Vector positions;
	Int        count  = 1000;
	Float      radius = 1.5f;

	void update(SimpleRtsSimulation& sim) override;
	void render(Renderer& renderer) override;
	void spawnEntity(const SimpleRtsSimulation& sim);
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;
};
