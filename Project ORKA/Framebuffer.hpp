#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct Framebuffer {
	Area size = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);

	Index framebufferID = 0;

	GPUTexture colorTexture;
	GPUTexture normalTexture;
	GPUTexture positionTexture;
	GPUTexture depthTexture;
	GPUTexture materialIDTexture;

	void use();
	void create();
	void destroy();
	Float aspectRatio();
	void setAsTexture();
	void blitFramebuffer();
	void resize(Area resolution);
};