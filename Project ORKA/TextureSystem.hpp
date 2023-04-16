
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
	GPUTexture& currentTexture();
	void add(CPUTexture& cpuTexture);
	Index use(ResourceManager& resourceManager, Name name);
	void use(ResourceManager& resourceManager, Name name, Index slot);
};