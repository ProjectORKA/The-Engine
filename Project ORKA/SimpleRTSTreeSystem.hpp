#pragma once
#include "Basics.hpp"
#include "SimpleRtsSystem.hpp"
#include "Time.hpp"

struct Renderer;
struct SimpleRtsSimulation;

struct SimpleRtsTreeSystem : SimpleRtsSystem
{
	// constants
	const UInt  count                 = 100;
	const Float treeRadius            = 1.0f;
	const Float minGrownTreeHeight    = 1.0f;
	const Float maxGrownTreeHeight    = 3.0f;
	const Float minTreeLifeExpectancy = 90.0f;
	const Float maxTreeLifeExpectancy = 3600.0f;
	const Float treeSpread            = treeRadius * 3.0f;

	UInt  treeCount = 0;

	//tree data
	Vector<Float> ages;
	Vector<Float> scales;
	Vector<Float> maxHeights;
	Vec3Vector    positions;
	Vec2Vector    directions;
	Vector<Float> lifeExpectancies;

	Float calculateTreeSize(UInt id) const;
	void  reset(const SimpleRtsSimulation& sim);
	void  render(Renderer& renderer) override;
	void  update(SimpleRtsSimulation& sim) override;
	void  spawnTree(const SimpleRtsSimulation& sim);
	bool  doesCollide(Vec2 position, Float radius) const;
	void  create(const SimpleRtsSimulation& sim) override;
	void  destroy(const SimpleRtsSimulation& sim) override;
	void  cutTree(const SimpleRtsSimulation& sim, UInt id);
	void  forceSpawnTree(const SimpleRtsSimulation& sim, Vec2 position);
};
