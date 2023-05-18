#pragma once

#include "OcTreeSystem.hpp"

struct Renderer;
struct PlanetSystemPlayer;

struct OctreeNodeRenderData {
	OcTreeNode* equivalentOctreeNode = nullptr;
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

	void count() const;
	void destroy();
	void subdivide();
	void unsubdivide();
	void create(OcTreeNode& octreeNode);
	void update(PlanetSystemPlayer& player);
	void render(ResourceManager& resourceManager, Renderer& renderer) const;
	void renderWater(ResourceManager& resourceManager, Renderer& renderer) const;
	void updateWithoutSubdivision(PlanetSystemPlayer& player);
	void renderLevel(ResourceManager& resourceManager, UShort level, Renderer& renderer) const;
	void renderWaterLevel(ResourceManager& resourceManager, UShort level, Renderer& renderer) const;
};

struct OctreeRenderSystem {
	OctreeNodeRenderData root;

	Index waterTextureID = 0;

	void count() const;
	void destroy();
	void create(Renderer& renderer);
	void update(PlanetSystemPlayer& player);
	void renderLevel(ResourceManager& resourceManager, UShort level, Renderer& renderer) const;
};
