
#include "TextureSystem.hpp"

void GPUTexture::use()
{
	if (loaded) {
		if (multisampling == 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		}
		else {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
		}
	}
}
void GPUTexture::unload() {
	
	if (loaded) {
		if (multisampling == 0) {
			glBindTexture(GL_TEXTURE_2D, 0);
			glDeleteTextures(1, &textureID);
		}
		else {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
			glDeleteTextures(1, &textureID);
		}
		loaded = false;
	}
}
void GPUTexture::load(CPUTexture& cpuTexture) {

	if (!loaded) {
		wrapping = cpuTexture.wrapping;
		filter = cpuTexture.filter;
		multisampling = cpuTexture.multisampling;
		channels = cpuTexture.channels;
		dataType = cpuTexture.dataType;
		width = cpuTexture.width;
		height = cpuTexture.height;
#ifdef GRAPHICS_API_OPENGL
		//create texture
		glGenTextures(1, &textureID);

		//load data
		if (multisampling == 0) {
			glBindTexture(GL_TEXTURE_2D, textureID);

			if (cpuTexture.dataType == dataTypeByte) {
				if (channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, dataTypeByte, cpuTexture.bytePixels);
				if (channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataTypeByte, cpuTexture.bytePixels);
				if (channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, dataTypeByte, cpuTexture.bytePixels);
				if (channels == 5) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, dataTypeByte, cpuTexture.bytePixels);
			}
			if (cpuTexture.dataType == dataTypeFloat) {
				if (channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, dataTypeFloat, cpuTexture.floatPixels);
				if (channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataTypeFloat, cpuTexture.floatPixels);
				if (channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, dataTypeFloat, cpuTexture.floatPixels);
				if (channels == 5) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, dataTypeFloat, cpuTexture.floatPixels);
			}
		}
		else {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);

			if (channels == 1) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RED, width, height, false);
			if (channels == 3) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RGB, width, height, false);
			if (channels == 4) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RGBA, width, height, false);
			if (channels == 5) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_DEPTH_COMPONENT24, width, height, false);
		}

	}

	//set texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
	if (wrapping == border) {
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(Color(1.0f, 0.0f, 1.0f, 1.0f)));
	}

	//set texture filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
	//set up mip mapping
	if (filter == linearMM || filter == nearestMM) glGenerateMipmap(GL_TEXTURE_2D);

	loaded = true;
#endif
}
void GPUTexture::resize(Int newWidth, Int newHeight)
{
	width = newWidth;
	height = newHeight;

	width = max(1, newWidth);
	height = max(1, newHeight);
	//load data
	if (multisampling == 0) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		if (dataType == dataTypeByte) {
			if (channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, dataTypeByte, 0);
			if (channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataTypeByte, 0);
			if (channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, dataTypeByte, 0);
			if (channels == 5) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, dataTypeByte, 0);
		}
		if (dataType == dataTypeFloat) {
			if (channels == 1) glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, dataTypeFloat, 0);
			if (channels == 3) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, dataTypeFloat, 0);
			if (channels == 4) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, dataTypeFloat, 0);
			if (channels == 5) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, dataTypeFloat, 0);
		}
	}
	else {

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
		if (channels == 1) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RED, width, height, false);
		if (channels == 3) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RGB, width, height, false);
		if (channels == 4) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_RGBA, width, height, false);
		if (channels == 5) glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisampling, GL_DEPTH_COMPONENT24, width, height, false);
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
		currentIndex = gpuTextures.size() - 1;
		textureNames[cpuTexture.name] = currentIndex;
	}
}
void TextureSystem::use(Name name) {
	auto it = textureNames.find(name);
	if (it != textureNames.end()) {
		currentIndex = it->second;
		current().use();
	}
}
void TextureSystem::use(Index textureIndex)
{
	currentIndex = textureIndex;
	current().use();
}
void TextureSystem::resize(Int width, Int height)
{
	current().resize(width, height);
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

GPUTexture& TextureSystem::current()
{
	return gpuTextures[currentIndex];
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
void invertOnLoad(Bool invert)
{
	stbi_set_flip_vertically_on_load(invert);
}