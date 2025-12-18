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

void FramebufferTexture::resize(Renderer& renderer, const IVec2 area)
{
	texture.resize(renderer, area);
}

FramebufferAttachment FramebufferTexture::getAttachmentSlot() const
{
	return framebufferAttachmentSlot;
}

void FramebufferTexture::useTextureInSlot(Renderer& renderer, const TextureSlot textureSlot) const
{
	texture.useTextureInSlot(renderer, textureSlot);
}

void FramebufferTexture::create(Renderer& renderer, const IVec2 size, const WritePixelsFormat format, const DataType type, const FramebufferAttachment framebufferAttachmentSlot, const Wrapping wrapping)
{
	texture.load(renderer, size, format, type, wrapping);
	this->format                    = format;
	this->type                      = type;
	this->framebufferAttachmentSlot = framebufferAttachmentSlot;
}
