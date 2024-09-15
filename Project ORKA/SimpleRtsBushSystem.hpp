#pragma once
#include "Basics.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBushSystem : SimpleRtsSystem
{
	Int          count  = 5000;
	Float        radius = 1.0f;
	Vec2Vector positions;

	void update(SimpleRtsSimulation& sim) override;
	void render(Renderer& renderer) const override;
	void spawnEntity(const SimpleRtsSimulation& sim);
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;
	Bool doesCollide(const Vec2 position, const Float radius) const;
};
