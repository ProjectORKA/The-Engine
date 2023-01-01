
#include "CPUTexture.hpp"
#include "Math.hpp"

Float CPUTexture::getRed(UInt x, UInt y) {
	x = x % width;
	y = y % height;

	switch (dataType) {
	case dataTypeByte:  return Float(bytePixels	[xyToIndex(x, y, 0)]) / 255.0f; break;
	case dataTypeFloat: return floatPixels		[xyToIndex(x, y, 0)]; break;
	case dataTypeUInt:  return uIntPixels		[xyToIndex(x, y, 0)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getGreen(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 1)]) / 255.0f; break;
	case dataTypeFloat: return floatPixels[xyToIndex(x, y, 1)]; break;
	case dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 1)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getBlue(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 2)]) / 255.0f; break;
	case dataTypeFloat: return floatPixels[xyToIndex(x, y, 2)]; break;
	case dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 2)]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getAlpha(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte:  return Float(bytePixels[xyToIndex(x, y, 3)]) / 255.0f; break;
	case dataTypeFloat: return floatPixels[xyToIndex(x, y, 3)]; break;
	case dataTypeUInt:  return uIntPixels[xyToIndex(x, y, 3)]; break;
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
	
	if (loaded) {
		if (pixels)free(pixels);
		pixels = nullptr;
	}
}
void CPUTexture::load(Path path, Name name)
{
	logDebug(String("Loading texture: (").append(name.data).append(" | ").append(path.string()).append(")"));

	Image image = loadImage(path, 8, true);

	this->name = name;
	this->bytePixels = image.pixels;
	this->width = image.width;
	this->height = image.height;
	this->channels = image.channels;

	if (pixels) loaded = true; else logEvent("Texture not loaded! Searching for different file format...");
}
void CPUTexture::load(Engine & engine, Name name) {
	if (!loaded) {
		//search in resource manager
		auto it = engine.resourceManager.textureResources.find(name);
		if (it != std::end(engine.resourceManager.textureResources)) {
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

Index CPUTexture::xyToIndex(Int x, Int y, Int channel)
{
	return channels * (y * width + x) + channel;
}

CPUTexture::~CPUTexture()
{
	unload();
}