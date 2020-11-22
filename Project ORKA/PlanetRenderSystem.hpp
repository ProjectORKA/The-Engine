
#pragma once

#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "PlanetSystem.hpp"
#include "Uniforms.hpp"
#include "Camera.hpp"
#include "RenderObjectSystem.hpp"
#include "TerrainRenderSystem.hpp"


struct ChunkRenderInfo {
	Vec3 chunkOffset;
	ULLVec3 chunkLocation;
	UShort level;

	void create(WorldChunk& chunk, Vec3 chunkOffset, TerrainRenderSystem & terrainRenderSystem);
	void render(Uniforms & uniforms, RenderObjectSystem & renderObjectSystem, Bool chunkBorders);
};

struct PlanetRenderSystem {
	Bool chunkBorders = false;
	Bool worldDistortion = true;
	Float renderDistance = 3.0f;

	Vector<Vector<ChunkRenderInfo>> chunkLevels;
	TerrainRenderSystem terrainRenderSystem;


	void create();
	void update(WorldChunk & chunk, Camera & camera);
	void count();
	void render(Uniforms& uniforms, RenderObjectSystem& renderObjectSystem);
};