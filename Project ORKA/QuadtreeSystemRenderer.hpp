#pragma once

#include "GPUMesh.hpp"
#include "QuadtreeNode.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct QuadtreeNodeRenderData
{
	GpuMesh       terrainMesh;
	Float         nodeDrawDistance       = 0.0;
	Bool          drawn                  = false;
	Bool          subdivided             = false;
	Vec3          cameraPosition         = Vec3(0);
	QuadtreeNode* equivalentQuadtreeNode = nullptr;

	QuadtreeNodeRenderData* c00 = nullptr;
	QuadtreeNodeRenderData* c01 = nullptr;
	QuadtreeNodeRenderData* c10 = nullptr;
	QuadtreeNodeRenderData* c11 = nullptr;

	void subdivide();
	void count() const;
	void destroy(Renderer& renderer);
	void unsubdivide(Renderer& renderer);
	void renderTerrain(Renderer& renderer);
	void create(QuadtreeNode& quadtreeNode);
	void loadTerrainMesh(Renderer& renderer);
	void renderTerrainLevel(UShort level, Renderer& renderer);
	void update(Renderer& renderer, PlanetSystemPlayer& player);
	void updateWithoutSubdivision(Renderer& renderer, PlanetSystemPlayer& player);
};

struct QuadtreeRenderSystem
{
	QuadtreeNodeRenderData root;

	void count() const;
	void destroy(Renderer& renderer);
	void renderLevel(UShort level, Renderer& renderer);
	void update(Renderer& renderer, PlanetSystemPlayer& player);
};
