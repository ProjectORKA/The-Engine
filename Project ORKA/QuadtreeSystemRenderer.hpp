#pragma once

#include "GPUMesh.hpp"
#include "GPUTexture.hpp"
#include "ShaderSystem.hpp"
#include "PlanetCamera.hpp"
#include "QuadtreeSystem.hpp"
#include "TerrainRendering.hpp"
#include "RelativeChunkOffset.hpp"

struct QuadtreeNodeRenderData {
	
	//data
	GPUMesh terrainMesh;
	//[TODO] turn to heightmap
	//GPUTexture heightmap;
	Vec3 chunkOffset;
	Bool inDrawDistance = false;
	ULL renderedHeight = 0;

	//quadtree stuff
	SharedPointer<QuadtreeNode> parentNode = nullptr;
	Bool subdivided = false;
	SharedPointer<QuadtreeNodeRenderData> c00 = nullptr;
	SharedPointer<QuadtreeNodeRenderData> c01 = nullptr;
	SharedPointer<QuadtreeNodeRenderData> c10 = nullptr;
	SharedPointer<QuadtreeNodeRenderData> c11 = nullptr;

	void destroy();
	void subdivide();
	void unsubdivide();
	void update(PlanetCamera & camera);
	void render(ShaderSystem& shaderSystem);
	void updateWithoutSubdivision(PlanetCamera& camera);
	void create(SharedPointer<QuadtreeNode> quadtreeNode);
	void renderLevel(UShort level, ShaderSystem& shaderSystem);
};