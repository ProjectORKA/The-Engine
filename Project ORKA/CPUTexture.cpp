
#include "CPUTexture.hpp"

void CPUTexture::loadHDR(Path path, Name name) {
	//stbi_hdr_to_ldr_gamma(2.2f);
	//stbi_hdr_to_ldr_scale(1.0f);
	//float *data = stbi_loadf(filepath, &width, &height, &bytes, 3);
	if (pixels) loaded = true;
}
void CPUTexture::load(Path path, Name name)
{

	logEvent(String("Loading texture: (").append(name.data).append(" | ").append(path.string()).append(")"));
	this->name = name;
	pixels = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
	if (pixels) loaded = true;
}
void CPUTexture::unload()
{
	if (loaded) {
		if (pixels)free(pixels);
	}
}
CPUTexture::~CPUTexture()
{
	unload();
}