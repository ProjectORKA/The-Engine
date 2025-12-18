
#pragma once
#include "CpuTexture.hpp"
#include "GPUTexture.hpp"

struct SortrResource
{
	CpuTexture cpuTexture;
	GPUTexture gpuTexture;
	Index      index    = 0;
	Float      priority = 0;
	Path       path     = Path("");

	void destroy(Renderer& renderer);
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};
