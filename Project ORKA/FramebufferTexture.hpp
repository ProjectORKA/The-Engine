#pragma once

#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct FramebufferTexture
{
	void                                destroy();
	[[nodiscard]] Bool                  isColor() const;
	[[nodiscard]] TextureID             getGLID() const;
	void                                resize(Area area);
	[[nodiscard]] FramebufferAttachment getAttachmentSlot() const;
	void                                useTextureInSlot(TextureSlot textureSlot) const;
	void                                create(const Area size, const WritePixelsFormat format, const DataType type, const FramebufferAttachment framebufferAttachmentSlot, const Wrapping wrapping);
private:
	GPUTexture            texture;
	DataType              type                      = DataType::Float;
	WritePixelsFormat     format                    = WritePixelsFormat::RGBA;
	FramebufferAttachment framebufferAttachmentSlot = FramebufferAttachment::Color0;
};
