#pragma once

#include "GPUTexture.hpp"

struct TextureSystem
{
	Vector<GPUTexture> gpuTextures;
	Map<Name, Index>   textureNames;
	Index              currentTextureId = 0;

	GPUTexture& currentTexture();
	void        use(Index textureId);
	void        create(Renderer& renderer);
	void        destroy(Renderer& renderer);
	void        resize(Renderer& renderer, Area size);
	Index       use(Renderer& renderer, const Name& name);
	void        use(Renderer& renderer, const Name& name, Index slot);
	void        add(Renderer& renderer, const CpuTexture& cpuTexture);
};
 