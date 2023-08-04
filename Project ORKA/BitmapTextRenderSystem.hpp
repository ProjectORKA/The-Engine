#pragma once

#include "GPUMesh.hpp"
#include "CpuMesh.hpp"
#include "GPUTexture.hpp"
#include "Fonts.hpp"

struct Renderer;
struct ResourceManager;

enum class Alignment {
	start = 0,
	center = 1,
	end = 2,
	middle = 1,
	left = 0,
	right = 2,
	top = 2,
	bottom = 0,
};

struct BitmapTextRenderSystem {
	GpuMesh gpuText;
	CpuMesh cpuText;
	GpuTexture textTexture;

	void destroy();
	void create(ResourceManager& resourceManager, Renderer& renderer);
	void render(ResourceManager& resourceManager, Renderer& renderer, const String& text, Vec2 position, Alignment x,
	            Alignment y, FontStyle style);
};
