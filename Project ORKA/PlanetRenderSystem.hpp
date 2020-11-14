
#pragma once

#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "PlanetSystem.hpp"
#include "Uniforms.hpp"
#include "Camera.hpp"
#include "RenderObjectSystem.hpp"

struct ChunkRenderInfo {
	Vec3 chunkOffset;
	WorldChunk* chunk;

	void render(Uniforms & uniforms, RenderObjectSystem & renderObjectSystem, Bool chunkBorders);
};

struct PlanetRenderSystem {
	Bool chunkBorders = false;
	Bool worldDistortion = true;

	Vector<Vector<ChunkRenderInfo>> chunkLevels;
	Float renderDistance = 3.0f;

	void create();
	void update(WorldChunk & chunk, Camera & camera);
	void count();
	void render(Uniforms& uniforms, RenderObjectSystem& renderObjectSystem);
};