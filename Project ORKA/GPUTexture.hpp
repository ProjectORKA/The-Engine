
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
	Int dataType = dataTypeByte;
	Short channels = 4;
	UInt width = 1;
	UInt height = 1;
	Bool loaded = false;
	void unload();
	void resize(Area size);
	void use(Index textireID);
	void load(CPUTexture& cpuTexture);
	void load(UInt width, UInt height) {
		
	};
};