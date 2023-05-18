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
	Area size = Area(1);
	Index framebufferID = -1;
	Vector<FramebufferTexture> frameBufferTextures;

	Bool dynamicResize = true;
	Float relativeScale = 1;

	void read() const;
	void draw() const;
	void destroy();
	Float aspectRatio() const;
	void blitFramebuffer() const;
	void create(Area size);
	void resize(Area resolution);
	void setAsTexture(Index slot = 0) const;
	void add(UInt components, DataType type, UInt slot);
	Vector<UInt> readPixelsAtCenterUIntRGB(UInt attachment);
	Vector<UInt> readPixelsUIntRGB(UInt x, UInt y, UInt attachment) const;
};
