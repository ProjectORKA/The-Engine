#pragma once

#include "OctreeSystem.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct OctreeNodeRenderData {
	OctreeNode* equivalentOctreeNode = nullptr;
	//data
	Vec3 cameraPosition = Vec3(0);
	Bool inDrawDistance = false;

	Bool subdivided = false;
	//[TODO] turn into shared pointer
	OctreeNodeRenderData* c000 = nullptr;
	OctreeNodeRenderData* c001 = nullptr;
	OctreeNodeRenderData* c010 = nullptr;
	OctreeNodeRenderData* c011 = nullptr;
	OctreeNodeRenderData* c100 = nullptr;
	OctreeNodeRenderData* c101 = nullptr;
	OctreeNodeRenderData* c110 = nullptr;
	OctreeNodeRenderData* c111 = nullptr;

	void count();
	void destroy();
	void subdivide();
	void unsubdivide();
	void create(OctreeNode & octreeNode);
	void update(PlanetSystemPlayer& player);
	void render(Engine& engine, Renderer & renderer);
	void renderWater(Engine& engine, Renderer & renderer);
	void updateWithoutSubdivision(PlanetSystemPlayer & player);
	void renderLevel(Engine& engine, UShort level, Renderer & renderer);
	void renderWaterLevel(Engine& engine, UShort level, Renderer& renderer);
};

struct OctreeRenderSystem {
	OctreeNodeRenderData root;

	Index waterTextureID = 0;

	void count();
	void destroy();
	void create(Renderer& renderer);
	void update(PlanetSystemPlayer& player);
	void renderLevel(Engine& engine, UShort level, Renderer& renderer);
};