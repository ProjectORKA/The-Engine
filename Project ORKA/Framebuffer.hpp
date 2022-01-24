#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct Framebuffer {
	Area size;
	Index framebufferID = 0;
	GPUTexture colorTexture;
	GPUTexture normalTexture;
	GPUTexture positionTexture;
	GPUTexture depthTexture;
	GPUTexture materialIDTexture;

	void use();
	void destroy();
	Float aspectRatio();
	void setAsTexture();
	void blitFramebuffer();
	void create(Area size);
	void resize(Area resolution);
};