#pragma once

#include "Basics.hpp"

struct Renderer;
struct SimpleRtsSimulation;

struct SimpleRtsSystem
{
	virtual void update(SimpleRtsSimulation& sim) = 0;
	virtual void render(Renderer& renderer) const = 0;
	virtual void create(const SimpleRtsSimulation& sim) = 0;
	virtual void destroy(const SimpleRtsSimulation& sim) = 0;
};

struct SimpleRtsEntitySystem : SimpleRtsSystem
{
	Vector<Vec2> positions;

	virtual Name       getEntityName() const = 0;
	virtual Int        getEntityCount() const = 0;
	virtual Float      getEntityRadius() const = 0;
	void               render(Renderer& renderer) const override;
	[[nodiscard]] Bool doesCollide(Vec2 position, Float radius) const;
	void               create(const SimpleRtsSimulation& sim) override;
	virtual void       spawnEntity(const SimpleRtsSimulation& sim) = 0;
};
