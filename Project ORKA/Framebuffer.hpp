#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct Framebuffer {
	Area size = Area(DEFAULT_WINDOW_WIDTH,DEFAULT_WINDOW_HEIGHT);
	Float aspectRatio = Float(DEFAULT_WINDOW_WIDTH) / Float(DEFAULT_WINDOW_HEIGHT);

	Color backgroundColor = Color(0.05f, 0.05f, 0.05f, 0.75f);

	Index framebufferID = 0;

	GPUTexture colorTexture;
	GPUTexture normalTexture;
	GPUTexture positionTexture;
	GPUTexture materialIDTexture;
	GPUTexture objectIDTexture;
	GPUTexture depthTexture;

	void create();
	void destroy();
	void setAsTexture();
	void blitFramebuffer(); //legacy
	void resize(Area resolution);
	void use(Renderer & renderer);
};