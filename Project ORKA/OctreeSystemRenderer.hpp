#pragma once

#include "OctreeSystem.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct OctreeNodeRenderData
{
	OctreeNode* equivalentOctreeNode = nullptr;
	// data
	Vec3 cameraPosition = Vec3(0);
	Bool inDrawDistance = false;

	Bool subdivided = false;
	// [TODO] turn into shared pointer
	OctreeNodeRenderData* c000 = nullptr;
	OctreeNodeRenderData* c001 = nullptr;
	OctreeNodeRenderData* c010 = nullptr;
	OctreeNodeRenderData* c011 = nullptr;
	OctreeNodeRenderData* c100 = nullptr;
	OctreeNodeRenderData* c101 = nullptr;
	OctreeNodeRenderData* c110 = nullptr;
	OctreeNodeRenderData* c111 = nullptr;

	void destroy();
	void subdivide();
	void count() const;
	void unsubdivide();
	void create(OctreeNode& octreeNode);
	void update(PlanetSystemPlayer& player);
	void updateWithoutSubdivision(PlanetSystemPlayer& player);
	void render(Renderer& renderer) const;
	void renderWater(Renderer& renderer) const;
	void renderLevel(UShort level, Renderer& renderer) const;
	void renderWaterLevel(UShort level, Renderer& renderer) const;
};

struct OctreeRenderSystem
{
	OctreeNodeRenderData root;
	Index                waterTextureId = 0;

	void destroy();
	void count() const;
	void create(Renderer& renderer) const;
	void update(PlanetSystemPlayer& player);
	void renderLevel(UShort level, Renderer& renderer) const;
};
