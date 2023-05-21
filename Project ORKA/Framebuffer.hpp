#pragma once

#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct FramebufferTexture
{
	UInt slot = 0;
	GpuTexture texture;
	Bool isColor = true;

	void destroy();
	FramebufferTexture();
	void resize(Area area);
};

struct Framebuffer
{
	String name = "Framebuffer";
	Area size = Area(1);
	Index framebufferId = -1;
	Vector<FramebufferTexture> framebufferTextures;

	Bool dynamicResize = true;
	Float relativeScale = 1;

	void read() const;
	void draw() const;
	void destroy();
	Float aspectRatio() const;
	void blitFramebuffer() const;
	void resize(Area resolution);
	void setAsTexture(Index slot = 0) const;
	void create(const String& name, Area dimensions);
	void add(UInt components, DataType type, UInt slot);
	Vector<UInt> readPixelsAtCenterUIntRgb(UInt attachment) const;
	Vector<UInt> readPixelsUIntRgb(UInt x, UInt y, UInt attachment) const;
};