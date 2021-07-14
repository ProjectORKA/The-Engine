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

	//quadtree stuff
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
	void render(ShaderSystem& shaderSystem, MeshSystem & meshSystem);
	void updateWithoutSubdivision(PlanetCamera& camera);
	void create(QuadtreeNode & quadtreeNode);
	void renderLevel(UShort level, ShaderSystem& shaderSystem, MeshSystem& meshsystem);
};

struct QuadtreeRenderSystem {

	Bool chunkBorders = false;
	Bool worldDistortion = true;
	Float drawDistance = 2.0f;

	//temporary data
	Index grassTextureID = 0;
	Index terrainShaderID = 0;

	QuadtreeNodeRenderData root;
	
	void count();
	void destroy();
	void create(Renderer & renderer);
	void render(PlanetSystem& planetSystem, Renderer& renderer, PlanetCamera& planetCamera);
};