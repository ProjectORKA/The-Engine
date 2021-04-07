#pragma once

#include "GPUTexture.hpp"

struct TextureSystem {

	CPUTexture cpuTexture;
	Index currentTextureID = 0;
	Map<Name, Index> textureNames;
	Vector<GPUTexture> gpuTextures;

	void add();
	void create();
	void destroy();
	void use(Name name);
	void use(Index textureID);
	void resize(Int width, Int height);

	GPUTexture& currentTexture();
};