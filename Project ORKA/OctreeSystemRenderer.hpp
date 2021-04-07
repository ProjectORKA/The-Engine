#pragma once

#include "GPUMesh.hpp"
#include "OctreeSystem.hpp"
#include "TerrainRendering.hpp"
#include "MeshSystem.hpp"
#include "TextureSystem.hpp"
#include "ShaderSystem.hpp"
#include "PlanetCamera.hpp"
#include "RelativeChunkOffset.hpp"

struct Renderer;

struct OctreeNodeRenderData {
	OctreeNode* parentNode;
	//data
	Vec3 chunkOffset = Vec3(0);

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

	void destroy();
	void subdivide();
	void unsubdivide();
	void create(OctreeNode & octreeNode);
	void update(PlanetCamera& camera, Float& renderDistance);
	void render(Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem);
	void renderAll(Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem);
	void renderLevel(UShort level, Bool chunkBorders, MeshSystem& meshSystem, TextureSystem& textureSystem, ShaderSystem& shaderSystem);
};