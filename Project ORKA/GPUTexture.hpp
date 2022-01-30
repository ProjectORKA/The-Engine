
#pragma once

#include "Basics.hpp"
#include "CPUTexture.hpp"
#include "GraphicsAPI.hpp"
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
	void load(Name name);
	void resize(Area size);
	void use(Index textireID);
	void attachTexture(Int slot);
	void load(CPUTexture& cpuTexture);
	void load(Vec2 size, Int channels, DataType type);
};