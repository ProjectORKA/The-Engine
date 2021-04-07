
#pragma once

#include "Basics.hpp"
#include "CPUTexture.hpp"
#include "GraphicsAPI.hpp"

struct GPUTexture {
	Index textureID = 0;
	Int filter = linearMM;
	Int wrapping = repeat;
	Short sampleCount = 0;
	Int dataType = GL_BYTE;
	Short channels = 4;
	UInt width = 1;
	UInt height = 1;
	Bool loaded = false;
	void load(CPUTexture& cpuTexture);
	void use();
	void resize(Int width, Int height);
	void unload();
};