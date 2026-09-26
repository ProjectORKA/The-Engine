#pragma once
#include "Basics.hpp"
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBushSystem : SimpleRtsSystem
{
	[[nodiscard]] Int  getCount() const;
	[[nodiscard]] Vec3 getPosition(Index index) const;
	void               update(SimpleRtsSimulation& sim) override;
	void               render(Renderer& renderer) override;
	void               spawnEntity(const SimpleRtsSimulation& sim);
	[[nodiscard]] Bool doesCollide(Vec2 position, Float radius) const;
	void               create(const SimpleRtsSimulation& sim) override;
	void               destroy(const SimpleRtsSimulation& sim) override;
	void               setPosition(const SimpleRtsSimulation& sim, Index index, Vec2 position);

private:
	Float      radius = 0.5f;
	Vec3Vector positions;
};
