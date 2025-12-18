#pragma once

#include "GPUTexture.hpp"

struct TextureSystem
{
	GPUTexture& currentTexture();
	void        create(Renderer& renderer);
	void        destroy(Renderer& renderer);
	void        resize(Renderer& renderer, IVec2 size);
	void        use(Renderer& renderer, Index textureId);
	Index       use(Renderer& renderer, const Name& name);
	void        use(Renderer& renderer, const Name& name, Index slot);
	void        add(Renderer& renderer, const CpuTexture& cpuTexture);
	
	private:
	Vector<GPUTexture> gpuTextures;
	Map<Name, Index>   textureNames;
	Index              currentTextureId = 0;
};
