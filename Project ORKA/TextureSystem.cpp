
#include "TextureSystem.hpp"
#include "Pixel.hpp"

void TextureSystem::create()
{	
	PixelByteRGBA* image = new PixelByteRGBA[4];

	image[0].red = 0;
	image[0].green = 0;
	image[0].blue = 0;
	image[0].alpha = 255;

	image[1].red = 0;
	image[1].green = 127;
	image[1].blue = 255;
	image[1].alpha = 255;

	image[2].red = 255;
	image[2].green = 127;
	image[2].blue = 0;
	image[2].alpha = 255;

	image[3].red = 0;
	image[3].green = 0;
	image[3].blue = 0;
	image[3].alpha = 255;


	CPUTexture defaultTexture;
	defaultTexture.channels = 4;
	defaultTexture.dataType = dataTypeByte;
	defaultTexture.nearFilter = Filter::nearest;
	defaultTexture.farFilter = Filter::nearest;
	defaultTexture.height = 2;
	defaultTexture.width = 2;
	defaultTexture.name = "empty";
	defaultTexture.wrapping = repeat;
	defaultTexture.bytePixels = &image->red;

	defaultTexture.loaded = true;

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
Index TextureSystem::use(Engine& engine, Name name)
{
	use(engine, name,0);
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
void TextureSystem::use(Engine& engine, Name name, Index slot) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		currentTextureID = it->second;
		currentTexture().use(slot);
	} else {
		
		CPUTexture cpuTexture;
		cpuTexture.load(engine, name);
		add(cpuTexture);

		it = textureNames.find(name);
		if (it != textureNames.end()) {
			currentTextureID = it->second;
			currentTexture().use(slot);
		}
		else {
			logError("Could not find Texture!");
			use(engine, "empty");
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