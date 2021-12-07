
#pragma once

#include "GPUTexture.hpp"
#include "GPUMesh.hpp"
#include "Fonts.hpp"

struct Renderer;

struct BitmapTextRenderSystem {
	GPUMesh gpuText;
	CPUMesh cpuText;
	GPUTexture textTexture;

	Renderer* rendererPtr = nullptr;

	void destroy();
	void create(Renderer& renderer);
	void render(String text, Float xPos, Float yPos, FontStyle style);
};