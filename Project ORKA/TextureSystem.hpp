#pragma once

#include "GPUTexture.hpp"

struct TextureSystem
{
	Vector<GPUTexture> gpuTextures;
	Map<Name, Index>   textureNames;
	Index              currentTextureId = 0;

	void        create();
	void        destroy();
	GPUTexture& currentTexture();
	void        resize(Area size);
	void        use(Index textureId);
	void        add(const CPUTexture& cpuTexture);
	Index       use(const Name& name);
	void        use(const Name& name, Index slot);
};
