
#include "CPUTexture.hpp"
#include "FileSystem.hpp"

void CPUTexture::unload()
{
	logDebug(String("Unloading Texture: (").append(name.data).append(")"));
	
	if (loaded) {
		if (pixels)free(pixels);
		pixels = nullptr;
	}
}
Float CPUTexture::getRed(UInt x, UInt y) {
	x = x % width;
	y = y % height;

	switch (dataType) {
	case dataTypeByte: return Float(bytePixels[channels * (y * width + x) + 0]) / 255; break;
	case dataTypeFloat: return floatPixels[channels * (y * width + x) + 0]; break;
	case dataTypeUInt: return uIntPixels[channels * (y * width + x) + 0]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getGreen(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte: return bytePixels[channels * (y * width + x) + 1]; break;
	case dataTypeFloat: return floatPixels[channels * (y * width + x) + 1]; break;
	case dataTypeUInt: return uIntPixels[channels * (y * width + x) + 1]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getBlue(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte: return bytePixels[channels * (y * width + x) + 2]; break;
	case dataTypeFloat: return floatPixels[channels * (y * width + x) + 2]; break;
	case dataTypeUInt: return uIntPixels[channels * (y * width + x) + 2]; break;
	default: logError("Not supported!"); break;
	}
}
Float CPUTexture::getAlpha(UInt x, UInt y) {
	switch (dataType) {
	case dataTypeByte: return bytePixels[channels * (y * width + x) + 3]; break;
	case dataTypeFloat: return floatPixels[channels * (y * width + x) + 3]; break;
	case dataTypeUInt: return uIntPixels[channels * (y * width + x) + 3]; break;
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
void CPUTexture::load(Name name) {
	if (!loaded)load(String("Data/textures/").append(name.data).append(".png"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".hdr"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".jpg"), name);
	if (!loaded)load(String("Data/textures/").append(name.data).append(".bmp"), name);
	//etc
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

CPUTexture::~CPUTexture()
{
	unload();
}