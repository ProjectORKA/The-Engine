
#include "Renderer.hpp"
#include "SortrResource.hpp"

void SortrResource::destroy(Renderer& renderer)
{
	cpuTexture.unload();
	gpuTexture.unload(renderer);
}

void SortrResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const auto  b    = static_cast<Float>(resourceCount);
	const auto  a    = static_cast<Float>(currentImageIndex);
	const auto  x    = static_cast<Float>(index);
	const Float diff = abs(a - x);
	priority         = b / 2.0f - min(abs(a - x - b), min(diff, abs(a - x + b)));
}