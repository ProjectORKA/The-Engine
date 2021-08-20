#pragma once

//#include "GPUMesh.hpp"
//#include "GPUTexture.hpp"
//#include "ShaderSystem.hpp"
//#include "QuadtreeSystem.hpp"
//#include "TerrainRendering.hpp"
//#include "RelativeChunkOffset.hpp"

#include "TextureSystem.hpp"
#include "ShaderSystem.hpp"
#include "PlanetCamera.hpp"
#include "MeshSystem.hpp"
#include "PlanetSystem.hpp"

struct Renderer;

struct QuadtreeNodeRenderData {
	
	GPUMesh terrainMesh;

	Vec3 chunkOffset;
	Bool inDrawDistance = false;
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
	void update(PlanetCamera & camera);
	void renderTerrain(Renderer& renderer);
	void create(QuadtreeNode & quadtreeNode);
	void updateWithoutSubdivision(PlanetCamera& camera);
	void renderTerrainLevel(UShort level, Renderer &renderer);
};

struct QuadtreeRenderSystem {
	Index grassTextureID = 0;
	Index terrainShaderID = 0;
	Index waterTextureID = 0;

	QuadtreeNodeRenderData root;

	QuadtreeNodeRenderData renderData[MAX_CHUNK_LEVEL][3][3];
	
	void count();
	void destroy();
	void update(PlanetCamera& camera);
	void renderLevel(UShort level, Renderer& renderer);
	void create(Renderer & renderer);
};