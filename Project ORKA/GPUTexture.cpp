#include "GPUTexture.hpp"
#include "Math.hpp"

void createTexture(const CPUTexture& cpuTexture) {
	switch (cpuTexture.channels) {
		case 1:
			switch (cpuTexture.dataType) {
				case DataType::dataTypeByte: apiTexImage2D(GL_TEXTURE_2D, 0, GL_R8, cpuTexture.width, cpuTexture.height,
				                                           0, GL_RED, GL_UNSIGNED_BYTE, cpuTexture.bytePixels.data());
					break;
				case DataType::dataTypeFloat: apiTexImage2D(
						GL_TEXTURE_2D, 0, GL_R32F, cpuTexture.width, cpuTexture.height, 0, GL_RED, GL_FLOAT,
						cpuTexture.floatPixels.data());
					break;
				case DataType::dataTypeUInt: apiTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT,
				                                           cpuTexture.uIntPixels.data());
					break;
				default: logError("Format not supported!");
					break;
			}
			break;
		case 2:
			switch (cpuTexture.dataType) {
				case DataType::dataTypeByte: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RG, GL_UNSIGNED_BYTE,
				                                           cpuTexture.bytePixels.data());
					break;
				case DataType::dataTypeFloat: apiTexImage2D(
						GL_TEXTURE_2D, 0, GL_RG32F, cpuTexture.width, cpuTexture.height, 0, GL_RG, GL_FLOAT,
						cpuTexture.floatPixels.data());
					break;
				case DataType::dataTypeUInt: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RG_INTEGER, GL_UNSIGNED_INT,
				                                           cpuTexture.uIntPixels.data());
					break;
				default: logError("Format not supported!");
					break;
			}
			break;
		case 3:
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			switch (cpuTexture.dataType) {
				case DataType::dataTypeByte: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				                                           cpuTexture.bytePixels.data());
					break;
				case DataType::dataTypeFloat: apiTexImage2D(
						GL_TEXTURE_2D, 0, GL_RGB32F, cpuTexture.width, cpuTexture.height, 0, GL_RGB, GL_FLOAT,
						cpuTexture.floatPixels.data());
					break;
				case DataType::dataTypeUInt: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT,
				                                           cpuTexture.uIntPixels.data());
					break;
				default: logError("Format not supported!");
					break;
			}
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			break;
		case 4:
			switch (cpuTexture.dataType) {
				case DataType::dataTypeByte: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				                                           cpuTexture.bytePixels.data());
					break;
				case DataType::dataTypeFloat: apiTexImage2D(
						GL_TEXTURE_2D, 0, GL_RGBA32F, cpuTexture.width, cpuTexture.height, 0, GL_RGBA, GL_FLOAT,
						cpuTexture.floatPixels.data());
					break;
				case DataType::dataTypeUInt: apiTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, cpuTexture.width,
				                                           cpuTexture.height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_INT,
				                                           cpuTexture.uIntPixels.data());
					break;
				default: logError("Format not supported!");
					break;
			}
			break;
		case 5:
			apiTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, cpuTexture.width, cpuTexture.height, 0,
			              GL_DEPTH_COMPONENT, GL_FLOAT, cpuTexture.floatPixels.data());
			break;
		default: logError("Invalid value for number of channels for Texture!");
	}
}

void GPUTexture::unload() {
	if (loaded) {
		apiBindTexture(GL_TEXTURE_2D, 0);
		apiDeleteTexture(textureID);
		loaded = false;
	}
}

void GPUTexture::load(ResourceManager& resourceManager, const Name& name) {
	CPUTexture t;
	t.load(resourceManager, name, Filter::linear, Filter::linearMM, Wrapping::repeat);
	load(t);
}

void GPUTexture::setFilters(Filter nearFilter, Filter farFilter) {
	this->nearFilter = nearFilter;
	this->farFilter = farFilter;
	apiBindTexture(GL_TEXTURE_2D, textureID);
	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<int>(nearFilter));
	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<int>(farFilter));
}

void GPUTexture::resize(Area size) {
	//[IMPORTANT] apply same stuff as in creation

	size.setMinimum(1);

	width = size.x;
	height = size.y;

	apiBindTexture(GL_TEXTURE_2D, textureID);

	CPUTexture tex;
	tex.channels = channels;
	tex.dataType = dataType;
	tex.width = width;
	tex.height = height;

	createTexture(tex);
}

void GPUTexture::use(const Index textureSlot) const {
	if (loaded) {
		switch (textureSlot) {
			case 0: apiActiveTexture(GL_TEXTURE0);
				break;
			case 1: apiActiveTexture(GL_TEXTURE1);
				break;
			case 2: apiActiveTexture(GL_TEXTURE2);
				break;
			case 3: apiActiveTexture(GL_TEXTURE3);
				break;
			case 4: apiActiveTexture(GL_TEXTURE4);
				break;
			case 5: apiActiveTexture(GL_TEXTURE5);
				break;
			case 6: apiActiveTexture(GL_TEXTURE6);
				break;
			case 7: apiActiveTexture(GL_TEXTURE7);
				break;
			case 8: apiActiveTexture(GL_TEXTURE8);
				break;
			case 9: apiActiveTexture(GL_TEXTURE9);
				break;
			case 10: apiActiveTexture(GL_TEXTURE10);
				break;
			case 11: apiActiveTexture(GL_TEXTURE11);
				break;
			case 12: apiActiveTexture(GL_TEXTURE12);
				break;
			case 13: apiActiveTexture(GL_TEXTURE13);
				break;
			case 14: apiActiveTexture(GL_TEXTURE14);
				break;
			case 15: apiActiveTexture(GL_TEXTURE15);
				break;
		}

		apiBindTexture(GL_TEXTURE_2D, textureID);
	}
}

void GPUTexture::attachTexture(const Int slot) const {
	apiBindTexture(GL_TEXTURE_2D, textureID);
	if (channels < 5) {
		switch (slot) {
			case 0: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
				break;
			case 1: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureID, 0);
				break;
			case 2: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textureID, 0);
				break;
			case 3: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, textureID, 0);
				break;
			case 4: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, textureID, 0);
				break;
			case 5: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, textureID, 0);
				break;
			case 6: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, textureID, 0);
				break;
			case 7: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT7, GL_TEXTURE_2D, textureID, 0);
				break;
			case 8: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT8, GL_TEXTURE_2D, textureID, 0);
				break;
			case 9: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT9, GL_TEXTURE_2D, textureID, 0);
				break;
			case 10: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT10, GL_TEXTURE_2D, textureID, 0);
				break;
			case 11: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT11, GL_TEXTURE_2D, textureID, 0);
				break;
			case 12: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT12, GL_TEXTURE_2D, textureID, 0);
				break;
			case 13: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT13, GL_TEXTURE_2D, textureID, 0);
				break;
			case 14: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT14, GL_TEXTURE_2D, textureID, 0);
				break;
			case 15: apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT15, GL_TEXTURE_2D, textureID, 0);
				break;
			default: logError("Texture slot not supported!");
				break;
		}
	}
	else { apiFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0); }
}

void GPUTexture::load(CPUTexture& cpuTexture) {
	if (loaded)unload();

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
		apiBindTexture(GL_TEXTURE_2D, textureID);

		createTexture(cpuTexture);

		//set texture wrapping
		apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<int>(wrapping));
		apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<int>(wrapping));
		if (wrapping == Wrapping::border) apiTexParameterfv(
			GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, value_ptr(Color(0.0f, 0.0f, 0.0f, 1.0f)));

		generateMipMaps(nearFilter, farFilter);

		loaded = true;
	}
	else { logWarning("CPUTexture not loaded!"); }
}

void GPUTexture::load(const Vec2 size, const Int channels, const DataType type) {
	CPUTexture cpuTexture;
	cpuTexture.name = "generated";
	cpuTexture.width = size.x;
	cpuTexture.height = size.y;
	cpuTexture.channels = channels;
	cpuTexture.dataType = type;
	cpuTexture.nearFilter = Filter::linear;
	cpuTexture.farFilter = Filter::linear;
	cpuTexture.wrapping = Wrapping::clamped;
	cpuTexture.loaded = true;
	load(cpuTexture);
}

void GPUTexture::generateMipMaps() const {
	//select tecture
	apiBindTexture(GL_TEXTURE_2D, textureID);

	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	//set up mip mapping
	apiGenerateMipmap(GL_TEXTURE_2D);
}

void GPUTexture::generateMipMaps(Filter nearFilter, Filter farFilter) const {
	//select tecture
	apiBindTexture(GL_TEXTURE_2D, textureID);

	//set texture filter
	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<int>(nearFilter));
	apiTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<int>(farFilter));
	apiTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1.0f);

	//set up mip mapping
	apiGenerateMipmap(GL_TEXTURE_2D);
}
