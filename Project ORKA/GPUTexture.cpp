
#include "GPUTexture.hpp"
#include "Math.hpp"
#include "Util.hpp"

void GPUTexture::unload() {
	if (loaded) {
		apiBindTexture(GL_TEXTURE_2D, 0);
		apiDeleteTexture(textureID);
		loaded = false;
	}
}
void GPUTexture::use(Index textureSlot)
{

	if (loaded) {
		switch (textureSlot) {
		case 0:	 apiActiveTexture(GL_TEXTURE0);  break;
		case 1:	 apiActiveTexture(GL_TEXTURE1);  break;
		case 2:	 apiActiveTexture(GL_TEXTURE2);  break;
		case 3:	 apiActiveTexture(GL_TEXTURE3);  break;
		case 4:	 apiActiveTexture(GL_TEXTURE4);  break;
		case 5:	 apiActiveTexture(GL_TEXTURE5);  break;
		case 6:	 apiActiveTexture(GL_TEXTURE6);  break;
		case 7:	 apiActiveTexture(GL_TEXTURE7);  break;
		case 8:	 apiActiveTexture(GL_TEXTURE8);  break;
		case 9:	 apiActiveTexture(GL_TEXTURE9);  break;
		case 10: apiActiveTexture(GL_TEXTURE10); break;
		case 11: apiActiveTexture(GL_TEXTURE11); break;
		case 12: apiActiveTexture(GL_TEXTURE12); break;
		case 13: apiActiveTexture(GL_TEXTURE13); break;
		case 14: apiActiveTexture(GL_TEXTURE14); break;
		case 15: apiActiveTexture(GL_TEXTURE15); break;
		}

		apiBindTexture(GL_TEXTURE_2D, textureID);
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
			apiGenTexture(textureID);

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
			apiBindTexture(GL_TEXTURE_2D, textureID);
			if (cpuTexture.dataType == dataTypeByte) {
				apiTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeByte, cpuTexture.bytePixels);
			}
			else {
				if (cpuTexture.dataType == dataTypeFloat) {
					apiTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeFloat, cpuTexture.floatPixels);
				}
				else {
					logError("Not datatype not supported!");
				}
			}

			//set texture wrapping
			apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
			apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
			if (wrapping == border) {
				apiTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(Color(0.0f, 0.0f, 0.0f, 1.0f)));
			}

			//set texture filter
			apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, enumClassAsInt(nearFilter));
			apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, enumClassAsInt(farFilter));
			apiTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);

			//set up mip mapping
			if (farFilter == Filter::linearMM || farFilter == Filter::nearestMM) apiGenerateMipmap(GL_TEXTURE_2D);

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
void GPUTexture::resize(Area size)
{
	if ((size.x == 0) | (size.y == 0))logError("Texture can't be resized to (0|0)!");

	width = size.x;
	height = size.y;

	UInt format = 0;

	switch (channels) {
	case 1:		format = GL_RED;				break;
	case 2:		format = GL_RG;					break;
	case 3:		format = GL_RGB;				break;
	case 4:		format = GL_RGBA;				break;
	case 5:		format = GL_DEPTH_COMPONENT;	break;
	default:	logError("Invalid value for number of channels for Texture!");
	}

	apiBindTexture(GL_TEXTURE_2D, textureID);
	if (dataType == dataTypeByte) {
		apiTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeByte, nullptr);
	}
	if (dataType == dataTypeFloat) {
		apiTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, dataTypeFloat, nullptr);
	}
}