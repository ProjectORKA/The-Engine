#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct FramebufferTexture {
	UInt slot = 0;
	GPUTexture texture;
	Bool isColor = true;

	void destroy();
	FramebufferTexture();
	void resize(Area area);
};

struct Framebuffer {
	Bool resizeable = false;
	Area size = Area(1);
	Index framebufferID = 0;
	Vector<FramebufferTexture> frameBufferTextures;

	void read();
	void draw();
	void destroy();
	Float aspectRatio();
	void blitFramebuffer();
	void create(Area size);
	void resize(Area resolution);
	void setAsTexture(Index slot);
	void add(UInt components, DataType type, UInt slot);
	Vector<UInt> readPixelsAtCenterUIntRGB(UInt attachment);
	Vector<UInt> readPixelsUIntRGB(UInt x, UInt y, UInt attachment);
};