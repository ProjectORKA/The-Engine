
#include "TextureSystem.hpp"

void TextureSystem::add()
{
	gpuTextures.emplace_back();
	gpuTextures.back().load(cpuTexture);
	currentTextureID = gpuTextures.size() - 1;
	textureNames[cpuTexture.name] = currentTextureID;
}
void TextureSystem::create()
{
	//loadAllTextures(*this);
}
void TextureSystem::destroy()
{
	cpuTexture.unload();

	for (GPUTexture& gpuTexture : gpuTextures) {
		gpuTexture.unload();
	}
	gpuTextures.clear();

	textureNames.clear();
}
void TextureSystem::use(Name name) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		currentTextureID = it->second;
		currentTexture().use();
	}
	else {
		cpuTexture.load(String("Data/textures/").append(name.data).append(".png"), name);
		add();
		it = textureNames.find(name);
		if (it != textureNames.end()) {
			currentTextureID = it->second;
			currentTexture().use();
		}
		else {
			logError("Could not find Texture!");
		}
	}
}
void TextureSystem::use(Index textureIndex)
{
	currentTextureID = textureIndex;
	currentTexture().use();
}
void TextureSystem::resize(Int width, Int height)
{
	currentTexture().resize(width, height);
}

GPUTexture& TextureSystem::currentTexture()
{
	return gpuTextures[currentTextureID];
}