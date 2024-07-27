#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"

constexpr auto voxelChunkSize   = 64;
constexpr auto voxelChunkHeight = 64;

struct Renderer;

struct Chunk
{
	Bool voxels[voxelChunkSize][voxelChunkSize][voxelChunkHeight];

	Chunk();
	CpuMesh generateMesh() const;
};

struct VoxelWorld
{
	Chunk chunk;
};

struct VoxelRenderer
{
	VoxelWorld world;
	GpuMesh    terrainMesh;

	void render(Renderer& renderer);
};
