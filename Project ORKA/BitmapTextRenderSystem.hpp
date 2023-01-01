
#pragma once

#include "GPUMesh.hpp"
#include "CPUMesh.hpp"
#include "GPUTexture.hpp"
#include "Fonts.hpp"

struct Renderer;
struct Engine;

struct BitmapTextRenderSystem {
	GPUMesh gpuText;
	CPUMesh cpuText;
	GPUTexture textTexture;

	void destroy();
	void create(Engine& engine, Renderer& renderer);
	void render(Engine& engine, Renderer& renderer, String text, Vec2 position, FontStyle style);
};