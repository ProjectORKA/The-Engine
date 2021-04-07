
#include "CPUTexture.hpp"

void CPUTexture::load(Path path, Name name)
{
	logEvent(String("Loading texture: (").append(name.data).append(" | ").append(path.string()).append(")"));
	this->name = name;
	pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
	if (pixels) loaded = true; else logError("Texture not loaded!");
}
void CPUTexture::unload()
{
	if (loaded) {
		if (pixels)free(pixels);
		pixels = nullptr;
	}
}
CPUTexture::~CPUTexture()
{
	unload();
}

void setInvertOnLoad(Bool invert)
{
	stbi_set_flip_vertically_on_load(invert);
}