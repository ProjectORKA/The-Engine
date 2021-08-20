#pragma once

#include "GPUMesh.hpp"
#include "PlanetSystem.hpp"
#include "TerrainRendering.hpp"
#include "MeshSystem.hpp"
#include "TextureSystem.hpp"
#include "ShaderSystem.hpp"
#include "PlanetCamera.hpp"
#include "RelativeChunkOffset.hpp"

struct Renderer;

struct OctreeNodeRenderData {
	OctreeNode* equivalentOctreeNode = nullptr;
	//data
	Vec3 chunkOffset = Vec3(0);
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
	void render(Renderer & renderer);
	void update(PlanetCamera& camera);
	void create(OctreeNode & octreeNode);
	void renderLevel(UShort level, Renderer & renderer);
	void updateWithoutSubdivision(PlanetCamera & camera);
	//
	void renderWater(Renderer & renderer);
	void renderWaterLevel(UShort level, Renderer& renderer);
};

struct OctreeRenderSystem {
	OctreeNodeRenderData root;

	Index waterTextureID = 0;

	void count();
	void destroy();
	void create(Renderer& renderer);
	void update(PlanetCamera& camera);
	void renderLevel(UShort level, Renderer& renderer);
};