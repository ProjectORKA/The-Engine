
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
	Int nearFilter = Filter::linear;
	Int farFilter = Filter::linearMM;

	void unload();
	void resize(Area size);
	void generateMipMaps();
	void use(Index textureID);
	void attachTexture(Int slot);
	void load(CPUTexture& cpuTexture);
	void load(Engine& engine, Name name);
	void load(Vec2 size, Int channels, DataType type);
	void generateMipMaps(Int nearFilter, Int farFilter);
};