
#pragma once

#include "Basics.hpp"
#include "GraphicsAPI.hpp"
#include "GPUTexture.hpp"
#include "GPUMesh.hpp"

struct Renderer;

struct TextRenderSystem {
	GPUMesh gpuText;
	CPUMesh cpuText;
	GPUTexture textTexture;

	Renderer * rendererPtr = nullptr;

	void destroy();
	void create(Renderer & renderer);
	void render(String text, Float xPos, Float yPos, Float size);
};