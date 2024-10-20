#pragma once
#include "CPUTexture.hpp"
#include "TiledMath.hpp"

struct GPUTexture
{
	[[nodiscard]] Bool              isLoaded() const;
	[[nodiscard]] WritePixelsFormat getFormat() const;
	[[nodiscard]] UInt              getOpenGLID() const;
	[[nodiscard]] Float             getAspectRatio() const;

	void unload();
	void resize(Area size);
	void load(const Name& name);
	void generateMipMaps() const;
	void load(const CpuTexture& cpuTexture);
	void useTextureInSlot(UInt textureSlot) const;
	void setFilters(Filter nearFilterValue, Filter farFilterValue);
	void generateMipMaps(Filter nearFilterValue, Filter farFilterValue);
	void load(IVec2 size, WritePixelsFormat format, DataType type, Wrapping wrapping);
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

	void setData(DataType dataType, WritePixelsFormat format, Int width, Int height, const Byte* data);
};
