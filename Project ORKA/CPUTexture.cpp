
#include "CPUTexture.hpp"

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
void setInvertOnLoad(Bool invert)
{
	stbi_set_flip_vertically_on_load(invert);
}
void CPUTexture::load(Path path, Name name)
{
	logEvent(String("Loading texture: (").append(name.data).append(" | ").append(path.string()).append(")"));
	this->name = name;
	bytePixels = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
	if (pixels) loaded = true; else logEvent("Texture not loaded! Searching for different file format...");
}

CPUTexture::~CPUTexture()
{
	unload();
}