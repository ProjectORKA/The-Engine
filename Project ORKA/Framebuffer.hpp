#pragma once

#include "FramebufferTexture.hpp"

#ifdef DEBUG
#define DEBUG_FRAMEBUFFER
#endif

struct Framebuffer
{
	void                destroy();
	void                clear() const;
	void                checkComplete();
	void                bindRead() const;
	void                bindDraw() const;
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	void                clearDepth() const;
	[[nodiscard]] Float aspectRatio() const;
	void                resize(Area resolution);
	void                clearColor(Vec4 clearColor) const;
	void                clearColor(IVec4 clearColor) const;
	void                setAsTexture(TextureSlot slot = 0) const;
	void                create(const String& name, Area dimensions);
	void                attachTexture(const FramebufferTexture& framebufferTexture);
	[[nodiscard]] IVec4 readPixelsAtCenterUIntRgb(FramebufferMode attachment) const;
	[[nodiscard]] IVec4 readPixelsUIntRgb(Int x, Int y, FramebufferMode attachment) const;
	void                add(WritePixelsFormat format, DataType type, FramebufferAttachment slot, Bool isDrawnTo, Wrapping wrapping);

	Bool complete = false; // [TODO] ponder
private:
	Area                       size = Area(1);
	Vector<UInt>               drawBuffers;
	String                     name = "EmptyFramebuffer";
	OpenGLFramebuffer          openglFramebuffer;
	Vector<FramebufferTexture> framebufferTextures;
};
