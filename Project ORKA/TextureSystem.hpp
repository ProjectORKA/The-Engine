#pragma once

#include "GraphicsAPI.hpp"
#include "BasicsModule.hpp"
#include "Debug.hpp"
#include "CPUTexture.hpp"
#include "HeightMap.hpp"

struct GPUTexture {
	Index textureID;
	Int filter = linearMM;
	Int wrapping = repeat;
	Bool loaded = false;

	void load(CPUTexture& cpuTexture);
	void render();
	void unload();
};

struct TextureSystem {
	Map<Name, Index> textureNames;

	Vector<CPUTexture> cpuTextures;
	Vector<GPUTexture> gpuTextures;

	void create();
	void add(CPUTexture& cpuTexture);
	void render(Name name);
	void render(Index textureID);
	void destroy();
};

void loadAllTextures(TextureSystem & textureSystem);