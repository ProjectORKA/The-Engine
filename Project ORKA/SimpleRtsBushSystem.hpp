#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsBushSystem : SimpleRtsEntitySystem
{
	Name  getEntityName() const override;
	Int   getEntityCount() const override;
	Float getEntityRadius() const override;
	void  update(const SimpleRtsSimulation& sim) override {};
	void  destroy(const SimpleRtsSimulation& sim) override {};
	void  spawnEntity(const SimpleRtsSimulation& sim) override;
};
