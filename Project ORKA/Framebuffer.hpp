#pragma once

#include "FramebufferTexture.hpp"

struct Framebuffer
{
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] Float aspectRatio() const;
	[[nodiscard]] IVec4 readPixelsAtCenterUIntRGB(FramebufferMode attachment) const;
	[[nodiscard]] IVec4 readPixelsUIntRGB(Int x, Int y, FramebufferMode attachment) const;

	void                destroy();
	void                clear() const;
	void                checkComplete();
	void                bindRead() const;
	void                bindDraw() const;
	void                clearDepth() const;
	void                resize(Area resolution);
	void                clearColor(Vec4 clearColor) const;
	void                clearColor(IVec4 clearColor) const;
	void                setAsTexture(TextureSlot slot = 0) const;
	void                create(const String& name, Area dimensions);
	void                attachTexture(const FramebufferTexture& framebufferTexture);
	void                add(WritePixelsFormat format, DataType type, FramebufferAttachment slot, Bool isDrawnTo, Wrapping wrapping);

	Bool complete = false; // [TODO] ponder
private:
	Area                       size = Area(1);
	Vector<UInt>               drawBuffers;
	String                     name = "EmptyFramebuffer";
	OpenGLFramebuffer          openglFramebuffer;
	Vector<FramebufferTexture> framebufferTextures;
};
