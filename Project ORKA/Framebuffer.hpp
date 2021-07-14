#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Framebuffer {
	Int width = DEFAULT_WINDOW_WIDTH;
	Int height = DEFAULT_WINDOW_HEIGHT;
	Float aspectRatio = Float(DEFAULT_WINDOW_WIDTH) / Float(DEFAULT_WINDOW_HEIGHT);

	Color backgroundColor = Color(0.05f, 0.05f, 0.05f, 0.75f);

	Index framebufferID = 0;

	GPUTexture colorTexture;
	GPUTexture depthTexture;

	void use();
	void create();
	void destroy();
	void setAsTexture();
	void detachTextures();
	void resize(Vec2 resolution);
	void attachTexture(GPUTexture& texture);


	void blitFramebuffer(); //legacy
};