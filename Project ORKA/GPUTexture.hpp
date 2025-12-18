#pragma once
#include "CPUTexture.hpp"
#include "TiledMath.hpp"

struct Renderer;

struct GPUTexture
{
	[[nodiscard]] Bool              isLoaded() const;
	[[nodiscard]] WritePixelsFormat getFormat() const;
	[[nodiscard]] UInt              getOpenGLID() const;
	[[nodiscard]] Float             getAspectRatio() const;

	void unload(Renderer & renderer);
	void resize(Renderer & renderer, IVec2 size);
	void generateMipMaps(Renderer& renderer) const;
	void load(Renderer& renderer, const Name& name);
	void load(Renderer& renderer, const CpuTexture& cpuTexture);
	void setFilters(Renderer& renderer, Filter nearFilterValue, Filter farFilterValue);
	void useTextureInSlot(Renderer& renderer, UInt textureSlot) const;
	void generateMipMaps(Renderer& renderer, Filter nearFilterValue, Filter farFilterValue);
	void load(Renderer& renderer, IVec2 size, WritePixelsFormat format, DataType type, Wrapping wrapping);
private:
	Int               width  = 1;
	Int               height = 1;
	Name              name   = "";
	OpenGLTexture2D   openglTexture;
	Bool              loaded     = false;
	Filter            nearFilter = Filter::Linear;
	Filter            farFilter  = Filter::LinearMm;
	Wrapping          wrapping   = Wrapping::Repeat;
	DataType          dataType   = DataType::Byte;
	WritePixelsFormat format     = WritePixelsFormat::RGBA;

	void setData(Renderer& renderer, DataType dataType, WritePixelsFormat format, Int width, Int height, const Byte* data);
};
