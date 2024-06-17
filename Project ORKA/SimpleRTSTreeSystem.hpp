#pragma once

#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"
#include "Time.hpp"

struct SimpleRtsSimulation;
struct Renderer;

struct SimpleRtsTreeSystem : SimpleRtsSystem
{
	// constants
	const UInt  count                 = 2000;
	const Float treeRadius            = 3.0f;
	const Float minGrownTreeHeight    = 1.0f;
	const Float maxGrownTreeHeight    = 3.0f;
	const Float minTreeLifeExpectancy = 90.0f;
	const Float maxTreeLifeExpectancy = 3600.0f;
	const Float treeSpread            = treeRadius * 3.0f;

	Timer timer;
	UInt  treeCount = 0;

	//tree data
	Vector<Float> ages;
	Vector<Float> scales;
	Vector<Float> maxHeights;
	Vector<Vec2>  positions;
	Vector<Vec2>  directions;
	Vector<Float> lifeExpectancies;

	Float calculateTreeSize(UInt id) const;
	void  render(Renderer& renderer) const override;
	void  reset(const SimpleRtsSimulation& sim);
	bool  doesCollide(Vec2 position, Float radius) const;
	void  spawnTree(const SimpleRtsSimulation& sim);
	void  destroy(const SimpleRtsSimulation& sim) override;
	void  update(const SimpleRtsSimulation& sim) override;
	void  create(const SimpleRtsSimulation& sim) override;
	void  cutTree(const SimpleRtsSimulation& sim, UInt id);
};
