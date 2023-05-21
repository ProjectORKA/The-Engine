#include "TextureSystem.hpp"

void TextureSystem::create() {
	Image image;
	image.height = 2;
	image.width = 2;
	image.channels = 4;
	image.bitCount = 8;
	image.pixels.resize(image.width * image.height * image.channels * image.bitCount / 8);

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

void TextureSystem::destroy() {
	for (GpuTexture& gpuTexture : gpuTextures) { gpuTexture.unload(); }
	gpuTextures.clear();

	textureNames.clear();
}

Index TextureSystem::use(ResourceManager& resourceManager, const Name& name) {
	use(resourceManager, name, 0);
	return currentTextureId;
}

void TextureSystem::resize(const Area size) { currentTexture().resize(size); }

void TextureSystem::use(const Index textureIndex) {
	currentTextureId = textureIndex;
	currentTexture().use(0);
}

void TextureSystem::use(ResourceManager& resourceManager, const Name& name, const Index slot) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		currentTextureId = it->second;
		currentTexture().use(slot);
	}
	else {
		CPUTexture cpuTexture;
		cpuTexture.load(resourceManager, name, Filter::nearest, Filter::linearMM, Wrapping::repeat);
		add(cpuTexture);

		it = textureNames.find(name);
		if (it != textureNames.end()) {
			currentTextureId = it->second;
			currentTexture().use(slot);
		}
		else {
			logError("Could not find Texture!");
			use(resourceManager, "empty");
		}
	}
}

void TextureSystem::add(CPUTexture& cpuTexture) {
	gpuTextures.emplace_back();
	gpuTextures.back().load(cpuTexture);
	currentTextureId = gpuTextures.size() - 1;
	textureNames[cpuTexture.name] = currentTextureId;
}

GpuTexture& TextureSystem::currentTexture() { return gpuTextures[currentTextureId]; }
