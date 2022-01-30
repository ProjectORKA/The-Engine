#pragma once

#include "Math.hpp"
#include "Settings.hpp"
#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct FramebufferTexture {
	Bool isColor = true;
	GPUTexture texture;
	UInt slot = 0;

	void destroy();
	FramebufferTexture();
	void resize(Area area);
};

struct Framebuffer {
	Area size = Area(1);
	Index framebufferID = 0;
	Vector<FramebufferTexture> textures;

	void use();
	Framebuffer();
	void destroy();
	~Framebuffer();
	Float aspectRatio();
	void blitFramebuffer();
	void resize(Area resolution);
	void setAsTexture(Index slot);
	void add(UInt components, DataType type, UInt slot);
};

struct GBuffer : public  Framebuffer {
	using Framebuffer::Framebuffer;
	GBuffer();
};

struct PixelIDs {
	UInt objectID = 0.0f;
	UInt instanceID = 0.0f;
	UInt primitiveID = 0.0f;
};

struct IDFrameBuffer : public Framebuffer {
	using Framebuffer::Framebuffer;
	IDFrameBuffer();
	PixelIDs getIDsAtCenter();
	PixelIDs getIDsAtLocation(UInt x, UInt y);
};