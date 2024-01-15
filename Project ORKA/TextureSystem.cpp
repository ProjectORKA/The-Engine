#include "TextureSystem.hpp"

void TextureSystem::create()
{
	Vector<Byte> pixels;
	pixels.resize(16);
	pixels[0]  = 0;
	pixels[1]  = 0;
	pixels[2]  = 0;
	pixels[3]  = 255;
	pixels[4]  = 0;
	pixels[5]  = 127;
	pixels[6]  = 255;
	pixels[7]  = 255;
	pixels[8]  = 255;
	pixels[9]  = 127;
	pixels[10] = 0;
	pixels[11] = 255;
	pixels[12] = 0;
	pixels[13] = 0;
	pixels[14] = 0;
	pixels[15] = 255;
	const Image image(2, 2, Channels::RGBA, ImageDataType::Byte, Memory(pixels.data(), pixels.size()));

	CPUTexture defaultTexture;

	defaultTexture.load(image, Filter::Nearest, Filter::NearestMm, Wrapping::Repeat, "DefaultTexture");

	add(defaultTexture);
}

void TextureSystem::destroy()
{
	for(GPUTexture& gpuTexture : gpuTextures) gpuTexture.unload();
	gpuTextures.clear();

	textureNames.clear();
}

void TextureSystem::resize(const Area size)
{
	currentTexture().resize(size);
}

GPUTexture& TextureSystem::currentTexture()
{
	return gpuTextures[currentTextureId];
}

void TextureSystem::use(const Index textureIndex)
{
	currentTextureId = textureIndex;
	currentTexture().useTextureInSlot(0);
}

void TextureSystem::add(const CPUTexture& cpuTexture)
{
	gpuTextures.emplace_back();
	gpuTextures.back().load(cpuTexture);
	currentTextureId                   = gpuTextures.size() - 1;
	textureNames[cpuTexture.getName()] = currentTextureId;
}

Index TextureSystem::use(const Name& name)
{
	use(name, 0);
	return currentTextureId;
}

void TextureSystem::use(const Name& name, const Index slot)
{
	auto it = textureNames.find(name);
	if(it != textureNames.end())
	{
		currentTextureId = it->second;
		currentTexture().useTextureInSlot(slot);
	}
	else
	{
		CPUTexture cpuTexture;

		cpuTexture.load(name, Filter::Linear, Filter::LinearMm, Wrapping::Repeat);

		add(cpuTexture);

		it = textureNames.find(name);
		if(it != textureNames.end())
		{
			currentTextureId = it->second;
			currentTexture().useTextureInSlot(slot);
		}
		else
		{
			logError("Could not find Texture!");
			use("empty");
		}
	}
}
