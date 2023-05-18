
#pragma once

#include "Math.hpp"
#include "GPUMesh.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "PerlinNoise.hpp"


struct SimpleRTSTerrainSystem {
	Float simpleRTSTerrainFunction(Vec2 position);
};

struct SimpleRTSTerrainRenderingSystem {
	GPUMesh gpuTerrain;
	CpuMesh cpuTerrain;
	Vec2 offset = Vec2(0);

	void create(ResourceManager& resourceManager);
	void render(Renderer& renderer);
	void update(SimpleRTSTerrainSystem& terrainSystem, Vec2 cameraPos);
};