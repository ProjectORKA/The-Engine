#pragma once
#include "Renderer.hpp"
#include "SimpleRtsSystem.hpp"

struct SimpleRtsSimulation;

struct SimpleRtsTerrainSystem : SimpleRtsSystem
{
	static constexpr Float seaLevel = 0.0f;

	CpuMesh terrainMesh;
	CpuMesh oceanMesh;

	[[nodiscard]] Double sampleHeight(const SimpleRtsSimulation& sim, Double a, Double b) const;
	[[nodiscard]] Bool isWater(const SimpleRtsSimulation& sim, Vec2 position) const;

	void update(SimpleRtsSimulation& sim) override {}
	void render(Renderer& renderer) override {}
	void create(const SimpleRtsSimulation& sim) override;
	void destroy(const SimpleRtsSimulation& sim) override;
};
