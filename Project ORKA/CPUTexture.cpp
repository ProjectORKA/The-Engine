#include "CPUTexture.hpp"
#include "Math.hpp"

Float CPUTexture::getRed(UInt x, UInt y) {
	x = x % width;
	y = y % height;

	switch (dataType) {
		case DataType::dataTypeByte: return static_cast<Float>(bytePixels[xyToIndex(x, y, 0)]) / 255.0f;
			break;
		case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 0)];
			break;
		case DataType::dataTypeUInt: return uIntPixels[xyToIndex(x, y, 0)];
			break;
		default: logError("Not supported!");
			break;
	}
}

Float CPUTexture::getGreen(const UInt x, const UInt y) {
	switch (dataType) {
		case DataType::dataTypeByte: return static_cast<Float>(bytePixels[xyToIndex(x, y, 1)]) / 255.0f;
			break;
		case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 1)];
			break;
		case DataType::dataTypeUInt: return uIntPixels[xyToIndex(x, y, 1)];
			break;
		default: logError("Not supported!");
			break;
	}
}

Float CPUTexture::getBlue(const UInt x, const UInt y) {
	switch (dataType) {
		case DataType::dataTypeByte: return static_cast<Float>(bytePixels[xyToIndex(x, y, 2)]) / 255.0f;
			break;
		case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 2)];
			break;
		case DataType::dataTypeUInt: return uIntPixels[xyToIndex(x, y, 2)];
			break;
		default: logError("Not supported!");
			break;
	}
}

Float CPUTexture::getAlpha(const UInt x, const UInt y) {
	switch (dataType) {
		case DataType::dataTypeByte: return static_cast<Float>(bytePixels[xyToIndex(x, y, 3)]) / 255.0f;
			break;
		case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 3)];
			break;
		case DataType::dataTypeUInt: return uIntPixels[xyToIndex(x, y, 3)];
			break;
		default: logError("Not supported!");
			break;
	}
}

Float CPUTexture::getRed(const Float x, const Float y) {
	//convert normalized coordinates to texture space
	const Float texX = x * static_cast<Float>(width);
	const Float texY = y * static_cast<Float>(height);

	//turn float to desired indices
	const UInt aX = floor(texX);
	const UInt aY = floor(texY);
	const UInt dX = ceil(texX);
	const UInt dY = ceil(texY);
	const UInt bX = dX;
	const UInt bY = aY;
	const UInt cX = aX;
	const UInt cY = dY;

	//get colors of adjacent pixels
	const Float a = getRed(aX, aY);
	const Float b = getRed(bX, bY);
	const Float c = getRed(cX, cY);
	const Float d = getRed(dX, dY);

	//get fraction to determine blending
	const Float fracX = fmod(texX, 1.0f);
	const Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}

Float CPUTexture::getGreen(const Float x, const Float y) {
	//convert normalized coordinates to texture space
	const Float texX = x * static_cast<Float>(width);
	const Float texY = y * static_cast<Float>(height);

	//turn float to desired indices
	const UInt aX = floor(texX);
	const UInt aY = floor(texY);
	const UInt dX = ceil(texX);
	const UInt dY = ceil(texY);
	const UInt bX = dX;
	const UInt bY = aY;
	const UInt cX = aX;
	const UInt cY = dY;

	//get colors of adjacent pixels
	const Float a = getGreen(aX, aY);
	const Float b = getGreen(bX, bY);
	const Float c = getGreen(cX, cY);
	const Float d = getGreen(dX, dY);

	//get fraction to determine blending
	const Float fracX = fmod(texX, 1.0f);
	const Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}

Float CPUTexture::getBlue(const Float x, const Float y) {
	//convert normalized coordinates to texture space
	const Float texX = x * static_cast<Float>(width);
	const Float texY = y * static_cast<Float>(height);

	//turn float to desired indices
	const UInt aX = floor(texX);
	const UInt aY = floor(texY);
	const UInt dX = ceil(texX);
	const UInt dY = ceil(texY);
	const UInt bX = dX;
	const UInt bY = aY;
	const UInt cX = aX;
	const UInt cY = dY;

	//get colors of adjacent pixels
	const Float a = getBlue(aX, aY);
	const Float b = getBlue(bX, bY);
	const Float c = getBlue(cX, cY);
	const Float d = getBlue(dX, dY);

	//get fraction to determine blending
	const Float fracX = fmod(texX, 1.0f);
	const Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}

Float CPUTexture::getAlpha(const Float x, const Float y) {
	//convert normalized coordinates to texture space
	const Float texX = x * static_cast<Float>(width);
	const Float texY = y * static_cast<Float>(height);

	//turn float to desired indices
	const UInt aX = floor(texX);
	const UInt aY = floor(texY);
	const UInt dX = ceil(texX);
	const UInt dY = ceil(texY);
	const UInt bX = dX;
	const UInt bY = aY;
	const UInt cX = aX;
	const UInt cY = dY;

	//get colors of adjacent pixels
	const Float a = getAlpha(aX, aY);
	const Float b = getAlpha(bX, bY);
	const Float c = getAlpha(cX, cY);
	const Float d = getAlpha(dX, dY);

	//get fraction to determine blending
	const Float fracX = fmod(texX, 1.0f);
	const Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}

void CPUTexture::unload() {
	if (!loaded) return;
	logDebug(String("Unloading Texture: (").append(name.data).append(")"));

	bytePixels.clear();
	floatPixels.clear();
	uIntPixels.clear();
}

void CPUTexture::load(const Image& image, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping,
                      const Name& name) {
	this->name = name;
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;
	this->farFilter = farFilter;
	this->nearFilter = nearFilter;
	this->wrapping = wrapping;
	this->name = name;

	switch (image.bitCount) {
		case 8:
			bytePixels.resize(image.width * image.height * image.channels);
			std::memcpy(bytePixels.data(), image.pixels.data(), bytePixels.size());
			dataType = DataType::dataTypeByte;
			loaded = true;
			break;
		case 32:
			floatPixels.resize(image.width * image.height * image.channels);
			std::memcpy(floatPixels.data(), image.pixels.data(), floatPixels.size() * 4);
			dataType = DataType::dataTypeFloat;
			loaded = true;
			break;
		default:
			logError("Unsupported bitCount!");
			break;
	}
}

void CPUTexture::load(ResourceManager& resourceManager, const Name& name, const Filter nearFilter,
                      const Filter farFilter, const Wrapping wrapping) {
	if (!loaded) { load(resourceManager.getTextureResourcePath(name), name, nearFilter, farFilter, wrapping); }
	else { logWarning("Texture already loaded, you are trying to load it again, something must be wrong!"); }
}

void CPUTexture::load(const Path& path, const Name& name, const Filter nearFilter, const Filter farFilter,
                      const Wrapping wrapping) {
	if (loaded) return;

	Image image;
	image.load(path, true);
	load(image, nearFilter, farFilter, wrapping, name);
}

void CPUTexture::load(const Path& path, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping) {
	const Name name = getFileName(path);
	load(path, name, nearFilter, farFilter, wrapping);
}

Index CPUTexture::xyToIndex(const Int x, const Int y, const Int channel) const {
	return channels * (y * width + x) + channel;
}

CPUTexture::CPUTexture() {}
CPUTexture::~CPUTexture() { unload(); }

CPUTexture::CPUTexture(const CPUTexture& other) {
	this->bytePixels = other.bytePixels;
	this->width = other.width;
	this->height = other.height;
	this->channels = other.channels;
	this->loaded = other.loaded;
	this->wrapping = other.wrapping;
	this->farFilter = other.farFilter;
	this->nearFilter = other.nearFilter;
	this->dataType = other.dataType;
	this->name = other.name;
}

CPUTexture& CPUTexture::operator=(const CPUTexture& other) {
	this->bytePixels = other.bytePixels;
	this->width = other.width;
	this->height = other.height;
	this->channels = other.channels;
	this->loaded = other.loaded;
	this->wrapping = other.wrapping;
	this->farFilter = other.farFilter;
	this->nearFilter = other.nearFilter;
	this->dataType = other.dataType;
	this->name = other.name;
	return *this;
}
