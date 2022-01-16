
#pragma once

#include "Basics.hpp"
#include "CPUTexture.hpp"
#include "GraphicsAPI.hpp"
#include "TiledMath.hpp"

struct GPUTexture {
	Index textureID = 0;
	Filter nearFilter = Filter::linear;
	Filter farFilter = Filter::linearMM;
	Int wrapping = repeat;
	DataType dataType = dataTypeByte;
	Short channels = 4;
	UInt width = 1;
	UInt height = 1;
	Bool loaded = false;
	void unload();
	void resize(Area size);
	void use(Index textireID);
	void attachTexture(Int slot);
	void load(CPUTexture& cpuTexture);
	void load(Vec2 size, Int channels, DataType type);
};