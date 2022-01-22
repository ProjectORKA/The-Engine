
#pragma once

#include "Basics.hpp"
#include "CPUMesh.hpp"
#include "GPUMesh.hpp"
#include "PerlinNoise.hpp"

#define VOXEL_CHUNK_SIZE 512
#define VOXEL_CHUNK_HEIGHT 64

struct Renderer;

struct Chunk {
	Bool voxels[VOXEL_CHUNK_SIZE][VOXEL_CHUNK_SIZE][VOXEL_CHUNK_HEIGHT];

	Chunk();
	CPUMesh generateMesh();
};

struct VoxelWorld {
	Chunk chunk;
};

struct VoxelRenderer {
	GPUMesh terrainMesh;

	void render(Renderer& renderer, VoxelWorld& world);
};