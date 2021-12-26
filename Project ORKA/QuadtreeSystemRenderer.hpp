#pragma once

#include "GPUMesh.hpp"
#include "QuadtreeNode.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct QuadtreeNodeRenderData {
	GPUMesh terrainMesh;

	Vec3 cameraPosition = Vec3(0);
	Bool drawn = false;
	Float nodeDrawDistance = 0.0;
	ULL renderedHeight = 0;

	QuadtreeNode * equivalentQuadtreeNode = nullptr;
	Bool subdivided = false;
	QuadtreeNodeRenderData * c00 = nullptr;
	QuadtreeNodeRenderData * c01 = nullptr;
	QuadtreeNodeRenderData * c10 = nullptr;
	QuadtreeNodeRenderData * c11 = nullptr;

	void count();
	void destroy();
	void subdivide();
	void unsubdivide();
	void loadTerrainMesh();
	void renderTerrain(Renderer& renderer);
	void update(PlanetSystemPlayer & player);
	void create(QuadtreeNode & quadtreeNode);
	void updateWithoutSubdivision(PlanetSystemPlayer& player);
	void renderTerrainLevel(UShort level, Renderer &renderer);
};

struct QuadtreeRenderSystem {
	QuadtreeNodeRenderData root;
	
	void count();
	void destroy();
	void update(PlanetSystemPlayer& player);
	void renderLevel(UShort level, Renderer& renderer);
};