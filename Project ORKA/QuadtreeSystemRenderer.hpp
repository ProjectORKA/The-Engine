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

	void destroy();
	void subdivide();
	void unsubdivide();
	void count() const;
	void loadTerrainMesh();
	void renderTerrain(Renderer& renderer);
	void update(PlanetSystemPlayer& player);
	void create(QuadtreeNode& quadtreeNode);
	void updateWithoutSubdivision(PlanetSystemPlayer& player);
	void renderTerrainLevel(UShort level, Renderer& renderer);
};

struct QuadtreeRenderSystem
{
	QuadtreeNodeRenderData root;

	void destroy();
	void count() const;
	void update(PlanetSystemPlayer& player);
	void renderLevel(UShort level, Renderer& renderer);
};
