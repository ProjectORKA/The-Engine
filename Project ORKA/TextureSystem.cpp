
#include "TextureSystem.hpp"

void TextureSystem::create()
{	
	Image image;
	image.height = 2;
	image.width = 2;
	image.channels = 4;
	image.bitcount = 8;
	image.pixels.resize(image.width * image.height * image.channels * image.bitcount / 8);

	image.pixels[0] = 0;
	image.pixels[1] = 0;
	image.pixels[2] = 0;
	image.pixels[3] = 255;
	image.pixels[4] = 0;
	image.pixels[5] = 127;
	image.pixels[6] = 255;
	image.pixels[7] = 255;
	image.pixels[8] = 255;
	image.pixels[9] = 127;
	image.pixels[10] = 0;
	image.pixels[11] = 255;
	image.pixels[12] = 0;
	image.pixels[13] = 0;
	image.pixels[14] = 0;
	image.pixels[15] = 255;

	CPUTexture defaultTexture;

	defaultTexture.load(image, Filter::nearest, Filter::nearestMM, Wrapping::repeat, "default");

	add(defaultTexture);
}
void TextureSystem::destroy()
{
	for (GPUTexture& gpuTexture : gpuTextures) {
		gpuTexture.unload();
	}
	gpuTextures.clear();

	textureNames.clear();
}
Index TextureSystem::use(ResourceManager& resourceManager, Name name)
{
	use(resourceManager, name,0);
	return currentTextureID;
}
void TextureSystem::resize(Area size)
{
	currentTexture().resize(size);
}
void TextureSystem::use(Index textureIndex)
{
	currentTextureID = textureIndex;
	currentTexture().use(0);
}
void TextureSystem::use(ResourceManager& resourceManager, Name name, Index slot) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		currentTextureID = it->second;
		currentTexture().use(slot);
	} else {
		
		CPUTexture cpuTexture;
		cpuTexture.load(resourceManager, name);
		add(cpuTexture);

		it = textureNames.find(name);
		if (it != textureNames.end()) {
			currentTextureID = it->second;
			currentTexture().use(slot);
		}
		else {
			logError("Could not find Texture!");
			use(resourceManager, "empty");
		}
	}
}
void TextureSystem::add(CPUTexture & cpuTexture)
{
	gpuTextures.emplaceBack();
	gpuTextures.last().load(cpuTexture);
	currentTextureID = gpuTextures.size() - 1;
	textureNames[cpuTexture.name] = currentTextureID;
}

GPUTexture& TextureSystem::currentTexture()
{
	return gpuTextures[currentTextureID];
}