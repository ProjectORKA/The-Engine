#include "FramebufferTexture.hpp"

void FramebufferTexture::destroy(Renderer& renderer)
{
	texture.unload(renderer);
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

void FramebufferTexture::resize(Renderer& renderer, const Area area)
{
	texture.resize(renderer, area);
}

FramebufferAttachment FramebufferTexture::getAttachmentSlot() const
{
	return framebufferAttachmentSlot;
}

void FramebufferTexture::useTextureInSlot(const TextureSlot textureSlot) const
{
	texture.useTextureInSlot(textureSlot);
}

void FramebufferTexture::create(Renderer& renderer, const Area size, const WritePixelsFormat format, const DataType type, const FramebufferAttachment framebufferAttachmentSlot, const Wrapping wrapping)
{
	texture.load(renderer, size, format, type, wrapping);
	this->format                    = format;
	this->type                      = type;
	this->framebufferAttachmentSlot = framebufferAttachmentSlot;
}
