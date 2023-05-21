#pragma once

#include "Basics.hpp"
#include "PerlinNoise.hpp"
#include "Renderer.hpp"
#include "Array2D.hpp"

#define CLIPMAPSIZE 256

struct ClipMap {
	CPUTexture heightmap;
	GpuTexture gpuHeightMap;

	GpuMesh terrain;


	void render(Renderer& renderer, Vec3 location) { }

	void create(Renderer& renderer, Vec3 location) {
		heightmap.channels = 1;
		heightmap.dataType = dataTypeFloat;
		heightmap.farFilter = Filter::linearMM;
		heightmap.nearFilter = Filter::linear;
		heightmap.height = CLIPMAPSIZE;
		heightmap.width = CLIPMAPSIZE;
		heightmap.name = "clipmap";
		heightmap.wrapping = Wrapping::repeat;
		heightmap.floatPixels = new Float[CLIPMAPSIZE * CLIPMAPSIZE];

		heightmap.loaded = true;
	}
};
