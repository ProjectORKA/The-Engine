
#include "GPUTexture.hpp"
#include "Math.hpp"
#include "Util.hpp"

void GPUTexture::unload() {
	if (loaded) {
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &textureID);
		loaded = false;
	}
}
void GPUTexture::use(Index textureSlot)
{

	if (loaded) {
		switch (textureSlot) {
		case 0:	 glActiveTexture(GL_TEXTURE0);  break;
		case 1:	 glActiveTexture(GL_TEXTURE1);  break;
		case 2:	 glActiveTexture(GL_TEXTURE2);  break;
		case 3:	 glActiveTexture(GL_TEXTURE3);  break;
		case 4:	 glActiveTexture(GL_TEXTURE4);  break;
		case 5:	 glActiveTexture(GL_TEXTURE5);  break;
		case 6:	 glActiveTexture(GL_TEXTURE6);  break;
		case 7:	 glActiveTexture(GL_TEXTURE7);  break;
		case 8:	 glActiveTexture(GL_TEXTURE8);  break;
		case 9:	 glActiveTexture(GL_TEXTURE9);  break;
		case 10: glActiveTexture(GL_TEXTURE10); break;
		case 11: glActiveTexture(GL_TEXTURE11); break;
		case 12: glActiveTexture(GL_TEXTURE12); break;
		case 13: glActiveTexture(GL_TEXTURE13); break;
		case 14: glActiveTexture(GL_TEXTURE14); break;
		case 15: glActiveTexture(GL_TEXTURE15); break;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
	}
}
void GPUTexture::load(CPUTexture& cpuTexture) {
	if (!loaded) {
		if (cpuTexture.loaded) {
	
			wrapping = cpuTexture.wrapping;
			nearFilter = cpuTexture.nearFilter;
			farFilter = cpuTexture.farFilter;
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

			//set texture wrapping
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
			if (wrapping == border) {
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(Color(0.0f, 0.0f, 0.0f, 1.0f)));
			}

			//set texture filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enumClassAsInt(nearFilter));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enumClassAsInt(farFilter));
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);
			//set up mip mapping
			if (farFilter == Filter::linearMM || farFilter == Filter::nearestMM) glGenerateMipmap(GL_TEXTURE_2D);

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
	case 1:		format = GL_RED;				break;
	case 2:		format = GL_RG;					break;
	case 3:		format = GL_RGB;				break;
	case 4:		format = GL_RGBA;				break;
	case 5:		format = GL_DEPTH_COMPONENT;	break;
	default:	logError("Invalid value for number of channels for Texture!");
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	if (dataType == dataTypeByte) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeByte, nullptr);
	}
	if (dataType == dataTypeFloat) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeFloat, 0);
	}
}