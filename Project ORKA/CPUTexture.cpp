
#include "CPUTexture.hpp"
#include "Math.hpp"

Float CPUTexture::getRed(UInt x, UInt y) {
	x = x % width;
	y = y % height;

	switch (dataType) {
	case DataType::dataTypeByte:  return Float(bytePixels	[xyToIndex(x, y, 0)]) / 255.0f; break;
	case DataType::dataTypeFloat: return floatPixels		[xyToIndex(x, y, 0)]; break;
	case DataType::dataTypeUInt:  return uIntPixels		[xyToIndex(x, y, 0)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getGreen(UInt x, UInt y) {
	switch (dataType) {
	case DataType::dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 1)]) / 255.0f; break;
	case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 1)]; break;
	case DataType::dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 1)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getBlue(UInt x, UInt y) {
	switch (dataType) {
	case DataType::dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 2)]) / 255.0f; break;
	case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 2)]; break;
	case DataType::dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 2)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getAlpha(UInt x, UInt y) {
	switch (dataType) {
	case DataType::dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 3)]) / 255.0f; break;
	case DataType::dataTypeFloat: return floatPixels[xyToIndex(x, y, 3)]; break;
	case DataType::dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 3)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getRed(Float x, Float y) {
	//convert normalized coordinates to texture space
	Float texX = x * Float(width);
	Float texY = y * Float(height);

	//turn float to desired indices
	UInt aX = floor(texX);
	UInt aY = floor(texY);
	UInt dX = ceil(texX);
	UInt dY = ceil(texY);
	UInt bX = dX;
	UInt bY = aY;
	UInt cX = aX;
	UInt cY = dY;

	//get colors of adjacent pixels
	Float a = getRed(aX, aY);
	Float b = getRed(bX, bY);
	Float c = getRed(cX, cY);
	Float d = getRed(dX, dY);

	//get fraction to determine blending
	Float fracX = fmod(texX, 1.0f);
	Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}
Float CPUTexture::getGreen(Float x, Float y) {
	//convert normalized coordinates to texture space
	Float texX = x * Float(width);
	Float texY = y * Float(height);

	//turn float to desired indices
	UInt aX = floor(texX);
	UInt aY = floor(texY);
	UInt dX = ceil(texX);
	UInt dY = ceil(texY);
	UInt bX = dX;
	UInt bY = aY;
	UInt cX = aX;
	UInt cY = dY;

	//get colors of adjacent pixels
	Float a = getGreen(aX, aY);
	Float b = getGreen(bX, bY);
	Float c = getGreen(cX, cY);
	Float d = getGreen(dX, dY);

	//get fraction to determine blending
	Float fracX = fmod(texX, 1.0f);
	Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}
Float CPUTexture::getBlue(Float x, Float y) {
	//convert normalized coordinates to texture space
	Float texX = x * Float(width);
	Float texY = y * Float(height);

	//turn float to desired indices
	UInt aX = floor(texX);
	UInt aY = floor(texY);
	UInt dX = ceil(texX);
	UInt dY = ceil(texY);
	UInt bX = dX;
	UInt bY = aY;
	UInt cX = aX;
	UInt cY = dY;

	//get colors of adjacent pixels
	Float a = getBlue(aX, aY);
	Float b = getBlue(bX, bY);
	Float c = getBlue(cX, cY);
	Float d = getBlue(dX, dY);

	//get fraction to determine blending
	Float fracX = fmod(texX, 1.0f);
	Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}
Float CPUTexture::getAlpha(Float x, Float y) {
	//convert normalized coordinates to texture space
	Float texX = x * Float(width);
	Float texY = y * Float(height);

	//turn float to desired indices
	UInt aX = floor(texX);
	UInt aY = floor(texY);
	UInt dX = ceil(texX);
	UInt dY = ceil(texY);
	UInt bX = dX;
	UInt bY = aY;
	UInt cX = aX;
	UInt cY = dY;

	//get colors of adjacent pixels
	Float a = getAlpha(aX, aY);
	Float b = getAlpha(bX, bY);
	Float c = getAlpha(cX, cY);
	Float d = getAlpha(dX, dY);

	//get fraction to determine blending
	Float fracX = fmod(texX, 1.0f);
	Float fracY = fmod(texY, 1.0f);

	//do linear interpolation to get the final blend
	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
}

void CPUTexture::unload()
{
	logDebug(String("Unloading Texture: (").append(name.data).append(")"));
	
	bytePixels.clear();
	floatPixels.clear();
	uIntPixels.clear();
}
void CPUTexture::load(Image image, Filter nearFilter, Filter farFilter, Wrapping wrapping, Name name) {
	this->name = name;
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;
	this->farFilter = farFilter;
	this->nearFilter = nearFilter;
	this->wrapping = wrapping;
	this->name = name;

	switch (image.bitcount) {
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
		logError("Unsupported bitcount!");
		break;
	}
}
void CPUTexture::load(ResourceManager& resourceManager, Name name) {
	if (!loaded) {
		//search in resource manager
		auto it = resourceManager.textureResources.find(name);
		if (it != std::end(resourceManager.textureResources)) {
			load(it->second, name);
		}
		else {
			logError(String("Texture ").append(name.data).append(" could not be found"));
		}
	}
	else {
		logWarning("Texture already loaded, you are trying to load it again, something must be wrong!");
	}
}
void CPUTexture::load(Path path, Name name)
{
	if (!loaded) {
		Image image;
		image.load(path, true);
		load(image,Filter::linear,Filter::linearMM,Wrapping::repeat, name);
	}
}
void CPUTexture::load(Path path)
{
	Name name = getFileName(path);
	load(path, name);
}
Index CPUTexture::xyToIndex(Int x, Int y, Int channel)
{
	return channels * (y * width + x) + channel;
}
CPUTexture::CPUTexture() {}
CPUTexture::~CPUTexture()
{
	unload();
}

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