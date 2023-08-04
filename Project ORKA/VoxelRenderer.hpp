#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"

#define VOXEL_CHUNK_SIZE 64
#define VOXEL_CHUNK_HEIGHT 64

struct Renderer;

struct Chunk
{
	Bool voxels[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_HEIGHT];

	Chunk();
	CPUMesh generateMesh(ResourceManager& resourceManager) const;
};

struct VoxelWorld
{
	Chunk chunk;
};

struct VoxelRenderer
{
	VoxelWorld world;
	GPUMesh    terrainMesh;

	void render(ResourceManager& resourceManager, Renderer& renderer);
};
