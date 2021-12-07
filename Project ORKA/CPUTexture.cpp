
#include "CPUTexture.hpp"
#include "FileSystem.hpp"

void CPUTexture::unload()
{
	logEvent("Unloading CPUTexture!");
	if (loaded) {
		if (pixels)free(pixels);
		pixels = nullptr;
	}
}
void CPUTexture::load(Name name) {
	if (!loaded)load(String("Data/textures/").append(name.data).append(".png"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".hdr"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".jpg"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".bmp"), name);
	//etc
}
void CPUTexture::load(Path path, Name name)
{
	logEvent(String("Loading texture: (").append(name.data).append(" | ").append(path.string()).append(")"));

	Image image = loadImage(path, 8, true);

	this->name = name;
	this->bytePixels = image.pixels;
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;

	if (pixels) loaded = true; else logEvent("Texture not loaded! Searching for different file format...");
}

CPUTexture::~CPUTexture()
{
	unload();
}