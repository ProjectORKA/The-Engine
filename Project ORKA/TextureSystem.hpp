#pragma once

#include "GraphicsAPI.hpp"
#include "Basics.hpp"
#include "Debug.hpp"
#include "CPUTexture.hpp"

struct GPUTexture {
	Index textureID = 0;
	Int filter = linearMM;
	Int wrapping = repeat;
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

struct TextureSystem {
	Index currentIndex = 0;

	Map<Name, Index> textureNames;

	Vector<CPUTexture> cpuTextures;
	Vector<GPUTexture> gpuTextures;

	void create();
	void add(CPUTexture& cpuTexture);
	void use(Name name);
	void use(Index textureID);
	void resize(Int width, Int height);
	void destroy();
	GPUTexture& current();
};

void loadAllTextures(TextureSystem & textureSystem);
void invertOnLoad(Bool invert);