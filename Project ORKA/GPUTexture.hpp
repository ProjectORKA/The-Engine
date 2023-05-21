#pragma once

#include "CPUTexture.hpp"
#include "TiledMath.hpp"

struct GpuTexture
{
	UInt width = 1;
	UInt height = 1;
	Byte channels = 4;
	Index textureId = 0;
	Bool loaded = false;
	Wrapping wrapping = Wrapping::repeat;
	DataType dataType = DataType::dataTypeByte;
	Filter nearFilter = Filter::linear;
	Filter farFilter = Filter::linearMM;

	void unload();
	void resize(Area size);
	void generateMipMaps() const;
	void use(Index textureSlot) const;
	void load(CPUTexture& cpuTexture);
	void attachTexture(Int slot) const;
	void load(Vec2 size, Int channels, DataType type);
	void setFilters(Filter nearFilter, Filter farFilter);
	void load(ResourceManager& resourceManager, const Name& name);
	void generateMipMaps(Filter nearFilter, Filter farFilter) const;
};