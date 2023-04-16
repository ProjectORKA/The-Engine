
#pragma once

#include "CPUTexture.hpp"
#include "TiledMath.hpp"

struct GPUTexture {
	UInt width = 1;
	UInt height = 1;
	Short channels = 4;
	Index textureID = 0;
	Bool loaded = false;
	Int wrapping = repeat;
	DataType dataType = dataTypeByte;
	Filter nearFilter = Filter::linear;
	Filter farFilter = Filter::linearMM;

	void unload();
	void resize(Area size);
	void generateMipMaps();
	void use(Index textureID);
	void attachTexture(Int slot);
	void load(CPUTexture& cpuTexture);
	void load(ResourceManager& resourceManager, Name name);
	void setFilters(Filter nearFilter, Filter farFilter) {
		this->nearFilter = nearFilter;
		this->farFilter = farFilter;
		apiBindTexture(GL_TEXTURE_2D, textureID);
		apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, nearFilter);
		apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, farFilter);
	};
	void load(Vec2 size, Int channels, DataType type);
	void generateMipMaps(Int nearFilter, Int farFilter);
};