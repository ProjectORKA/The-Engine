#pragma once

#include "GPUTexture.hpp"
#include "TiledMath.hpp"

struct FramebufferTexture
{
	[[nodiscard]] Bool                  isColor() const;
	[[nodiscard]] TextureID             getGLID() const;
	[[nodiscard]] FramebufferAttachment getAttachmentSlot() const;

	void destroy(Renderer& renderer);
	void resize(Renderer& renderer, IVec2 area);
	void useTextureInSlot(Renderer& renderer, TextureSlot textureSlot) const;
	void create(Renderer& renderer, IVec2 size, WritePixelsFormat format, DataType type, FramebufferAttachment framebufferAttachmentSlot, Wrapping wrapping);

private:
	GPUTexture            texture;
	DataType              type                      = DataType::Float;
	WritePixelsFormat     format                    = WritePixelsFormat::RGBA;
	FramebufferAttachment framebufferAttachmentSlot = FramebufferAttachment::Color0;
};
