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
	UInt objectID = -1;
	UInt instanceID = -1;
	UInt primitiveID = -1;
};

struct IDFrameBuffer : public Framebuffer {
	using Framebuffer::Framebuffer;

	PixelIDs currentIds;

	void update() {
		//this needs to be called after rendering all interactive objects to this framebuffer, using the idShader
		//it will get the currently hovered object IDs so that external threads can safely interact with them
		currentIds = getID();
	}
	IDFrameBuffer();
	PixelIDs getID();
	PixelIDs getIDsAtCenter();
	PixelIDs getIDUnderCursor();
	PixelIDs getIDsAtLocation(UInt x, UInt y);
};