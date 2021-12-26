#pragma once

#include "GPUTexture.hpp"

struct TextureSystem {
	Index currentTextureID = 0;
	Map<Name, Index> textureNames;
	Vector<GPUTexture> gpuTextures;

	void create();
	void destroy();
	void resize(Area size);
	void use(Index textureID);
	void use(Name name, Index slot);
	void add(CPUTexture& cpuTexture);

	Index use(Name name);
	GPUTexture& currentTexture();
};