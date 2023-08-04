#include "FramebufferTexture.hpp"
#include "Profiler.hpp"

void FramebufferTexture::destroy()
{
	texture.unload();
}

Bool FramebufferTexture::isColor() const
{
	if(framebufferAttachmentSlot != FramebufferAttachment::Depth && framebufferAttachmentSlot != FramebufferAttachment::DepthStencil && framebufferAttachmentSlot != FramebufferAttachment::Stencil) return true;
	return false;
}

TextureID FramebufferTexture::getGLID() const
{
	return texture.getOpenGLID();
}

void FramebufferTexture::resize(const Area area)
{
	texture.resize(area);
}

FramebufferAttachment FramebufferTexture::getAttachmentSlot() const
{
	return framebufferAttachmentSlot;
}

void FramebufferTexture::useTextureInSlot(const TextureSlot textureSlot) const
{
	texture.useTextureInSlot(textureSlot);
}

void FramebufferTexture::create(const Area size, const WritePixelsFormat format, const DataType type, const FramebufferAttachment framebufferAttachmentSlot, const Wrapping wrapping)
{
	texture.load(size, format, type, wrapping);
	this->format                    = format;
	this->type                      = type;
	this->framebufferAttachmentSlot = framebufferAttachmentSlot;
}
