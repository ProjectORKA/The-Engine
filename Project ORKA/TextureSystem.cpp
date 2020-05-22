
#include "TextureSystem.hpp"

void GPUTexture::load(CPUTexture& cpuTexture) {
	if (!loaded) {
		wrapping = cpuTexture.wrapping;
		filter = cpuTexture.filter;
#ifdef GRAPHICS_API_OPENGL
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		switch (cpuTexture.dataType) {
		case dataTypeByte:
			if (cpuTexture.channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cpuTexture.width, cpuTexture.height, 0, GL_RGB, dataTypeByte, cpuTexture.bytePixels);
			if (cpuTexture.channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cpuTexture.width, cpuTexture.height, 0, GL_RGBA, dataTypeByte, cpuTexture.bytePixels);
			break;
		case dataTypeFloat:
			break;
			if (cpuTexture.channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cpuTexture.width, cpuTexture.height, 0, GL_RGB, dataTypeFloat, cpuTexture.floatPixels);
			if (cpuTexture.channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cpuTexture.width, cpuTexture.height, 0, GL_RGBA, dataTypeFloat, cpuTexture.floatPixels);
		default:
			assert(0);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);

		if (filter == linearMM || filter == nearestMM) glGenerateMipmap(GL_TEXTURE_2D);

		loaded = true;
#endif
	}
}
void GPUTexture::render()
{
	if (loaded) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		glActiveTexture(GL_TEXTURE0);
		//glUniform1i(glGetUniformLocation(program, uniformName), 0);
	}
}
void GPUTexture::unload() {
	if (loaded) {
#ifdef GRAPHICS_API_OPENGL
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &textureID);
#endif // GRAPHICS_API_OPENGL
	}
}

void TextureSystem::create()
{
	loadAllTextures(*this);
}
void TextureSystem::add(CPUTexture& cpuTexture)
{
	if (cpuTexture.loaded) {
		gpuTextures.emplace_back();
		gpuTextures.back().load(cpuTexture);
		textureNames[cpuTexture.name] = gpuTextures.size() - 1;
	}
}
void TextureSystem::render(Name name) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		gpuTextures[it->second].render();
	}
}
void TextureSystem::render(Index textureID)
{
	gpuTextures[textureID].render();
}
void TextureSystem::destroy()
{
	for (CPUTexture& cpuTexture : cpuTextures) {
		cpuTexture.unload();
	}
	cpuTextures.clear();

	for (GPUTexture& gpuTexture : gpuTextures) {
		gpuTexture.unload();
	}
	gpuTextures.clear();

	textureNames.clear();
}

void loadAllTextures(TextureSystem& textureSystem)
{
	if (std::filesystem::exists(std::filesystem::current_path() / "Data/textures"))
		for (auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path() / "Data/textures")) {
			CPUTexture tmp;
			tmp.load(entry.path(), Name(entry.path().filename().replace_extension().string().c_str()));
			if (tmp.loaded) textureSystem.add(tmp);
		}
	else logError("Texture folder (Data/textures) doesent exist!");
}