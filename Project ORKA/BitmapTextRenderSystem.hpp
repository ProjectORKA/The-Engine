#pragma once

#include "GPUMesh.hpp"
#include "CPUMesh.hpp"
#include "GPUTexture.hpp"

struct Renderer;
struct ResourceManager;

enum class Alignment
{
	left   = 0,
	right  = 2,
	top    = 0,
	bottom = 2,
	middle = 1,
	center = 1,
};

struct BitmapTextRenderSystem
{
	GpuMesh    gpuText;
	CpuMesh    cpuText;
	GPUTexture textTexture;

	void destroy();
	void create(Renderer& renderer);
	void render(Renderer& renderer, const String& text, Vec2 position, Alignment x, Alignment y, Float absoluteSize, Float letterSpacing);
};
