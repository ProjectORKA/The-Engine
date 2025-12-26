#pragma once

#include "FramebufferTexture.hpp"

struct Renderer;

struct Framebuffer
{
	[[nodiscard]] IVec2 getSize() const;
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] Float aspectRatio() const;
	[[nodiscard]] IVec4 readPixelsAtCenterUIntRGB(Renderer& renderer, FramebufferMode attachment) const;
	[[nodiscard]] Vec4  readPixelsAtCenterVec4(Renderer& renderer, FramebufferMode attachment) const;
	[[nodiscard]] IVec4 readPixelsUIntRGB(Renderer& renderer, Int x, Int y, FramebufferMode attachment) const;
	[[nodiscard]] Vec4  readPixelsVec4(Renderer& renderer, Int x, Int y, FramebufferMode attachment) const;
	[[nodiscard]] Bool  isComplete() const;

	void destroy(Renderer& renderer);
	void clear(Renderer& renderer) const;
	void checkComplete(Renderer& renderer);
	void bindRead(Renderer& renderer) const;
	void bindDraw(Renderer& renderer) const;
	void clearDepth(Renderer& renderer) const;
	void resize(Renderer& renderer, IVec2 resolution);
	void clearColor(Renderer& renderer, Vec4 clearColor) const;
	void clearColor(Renderer& renderer, IVec4 clearColor) const;
	void setAsTexture(Renderer& renderer, TextureSlot slot = 0) const;
	void create(Renderer& renderer, const String& name, IVec2 dimensions);
	void attachTexture(Renderer& renderer, const FramebufferTexture& framebufferTexture);
	void add(Renderer& renderer, WritePixelsFormat format, DataType type, FramebufferAttachment slot, Bool isDrawnTo, Wrapping wrapping);

private:
	Vector<UInt>               drawBuffers;
	IVec2                      size = IVec2(1);
	Bool                       complete = false;
	OpenGLFramebuffer          openglFramebuffer;
	Vector<FramebufferTexture> framebufferTextures;
	String                     name = "EmptyFramebuffer";
};
