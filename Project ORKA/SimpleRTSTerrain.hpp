#pragma once

#include "GPUMesh.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"

struct SimpleRTSTerrainSystem
{
	[[nodiscard]] Float simpleRTSTerrainFunction(Vec2 position) const;
};

struct SimpleRTSTerrainRenderingSystem
{
	CPUMesh cpuTerrain;
	GPUMesh gpuTerrain;
	Vec2    offset = Vec2(0);

	void render(Renderer& renderer) const;
	void create(ResourceManager& resourceManager);
	void update(const SimpleRTSTerrainSystem& terrainSystem, Vec2 cameraPos);
};
