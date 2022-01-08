
#pragma once

#include "GPUTexture.hpp"
#include "GPUMesh.hpp"
#include "Fonts.hpp"

struct Renderer;

struct BitmapTextRenderSystem {
	GPUMesh gpuText;
	CPUMesh cpuText;
	GPUTexture textTexture;

	void destroy();
	void create(Renderer& renderer);
	void render(Renderer& renderer, String text, Vec2 position, FontStyle style);
};