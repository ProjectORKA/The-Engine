#pragma once

#include "GPUTexture.hpp"

struct TextureSystem {
	Index currentTextureId = 0;
	Map<Name, Index> textureNames;
	Vector<GpuTexture> gpuTextures;

	void create();
	void destroy();
	void resize(Area size);
	void use(Index textureId);
	GpuTexture& currentTexture();
	void add(CPUTexture& cpuTexture);
	Index use(ResourceManager& resourceManager, const Name& name);
	void use(ResourceManager& resourceManager, const Name& name, Index slot);
};
