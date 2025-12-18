#pragma once
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsSimulation;

struct SimpleRtsTerrainSystem : SimpleRtsSystem
{
	Int count = 200000;

	CpuMesh terrainMesh;
	UInt    terrainResolution = 128;

	Double terrainFunction(Double a, Double b, Double mapSize) const;

	[[nodiscard]] Bool isWater(const SimpleRtsSimulation& sim, Vec2 position) const;

	void update(SimpleRtsSimulation& sim) override {}
	void render(Renderer& renderer) override {}
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override {}
};
