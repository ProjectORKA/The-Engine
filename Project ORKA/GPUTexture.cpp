
#include "GPUTexture.hpp"
#include "Math.hpp"

#define FIXED_SAMPLE_LOCATIONS true

void GPUTexture::use()
{
	if (loaded) {
		if (sampleCount == 0) {
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
		if (sampleCount == 0) {
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
		if (cpuTexture.loaded) {

		wrapping = cpuTexture.wrapping;
		filter = cpuTexture.filter;
		sampleCount = cpuTexture.multisampling;
		channels = cpuTexture.channels;
		dataType = cpuTexture.dataType;
		width = cpuTexture.width;
		height = cpuTexture.height;

		//create texture
		glGenTextures(1, &textureID);

		UInt format = 0;

		switch (channels) {
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		case 5: format = GL_DEPTH_COMPONENT; break;
		default: logError("Invalid value for number of channels for Texture!");
		};

		//load data
		if (sampleCount == 0) {
			glBindTexture(GL_TEXTURE_2D, textureID);

			if (cpuTexture.dataType == dataTypeByte) {
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeByte, cpuTexture.bytePixels);
			}
			else {
				if (cpuTexture.dataType == dataTypeFloat) {
					glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeFloat, cpuTexture.floatPixels);
				}
				else {
					logError("Not datatype not supported!");
				}
			}
		}
		else {
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, format, width, height, FIXED_SAMPLE_LOCATIONS);
			//glTextureStorage2DMultisample(textureID, sampleCount, format, width, height, FIXED_SAMPLE_LOCATIONS);
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
		}
		else {
			logError("CPUTexture not loaded!");
		}
	}
	else {
		logError("GPUTexture already loaded!");
	}
}
void GPUTexture::resize(Int newWidth, Int newHeight)
{
	width = newWidth;
	height = newHeight;

	width = max(1, newWidth);
	height = max(1, newHeight);

	UInt format = 0;

	switch (channels) {
	case 1: format = GL_RED; break;
	case 2: format = GL_RG; break;
	case 3: format = GL_RGB; break;
	case 4: format = GL_RGBA; break;
	case 5: format = GL_DEPTH_COMPONENT; break;
	default: logError("Invalid value for number of channels for Texture!");
	}

	if (sampleCount == 0) {
		glBindTexture(GL_TEXTURE_2D, textureID);
		if (dataType == dataTypeByte) {
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeByte, nullptr);
		}
		if (dataType == dataTypeFloat) {
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeFloat, 0);
		}
	}
	else {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sampleCount, format, width, height, false);
	}
}