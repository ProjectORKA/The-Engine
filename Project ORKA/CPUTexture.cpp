#include "CPUTexture.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "Math.hpp"
#include "ResourceManager.hpp"

CpuTexture::~CpuTexture()
{
	unload();
}

Float CpuTexture::getAspectRatio() const
{
	if(width > 0.0f && height > 0.0f) return static_cast<Float>(width) / static_cast<Float>(height);
	return 1.0f;
}

void CpuTexture::unload()
{
	loaded = false;
	pixelMemory.destroy();
}

Int CpuTexture::getWidth() const
{
	if(width <= 0) logError("Width is 0");
	return width;
}

Name CpuTexture::getName() const
{
	return name;
}

Bool CpuTexture::isLoaded() const
{
	return loaded;
}

Int CpuTexture::getHeight() const
{
	if(width <= 0) logError("Height is 0");
	return height;
}

Filter CpuTexture::getFarFilter() const
{
	return farFilter;
}

Filter CpuTexture::getNearFilter() const
{
	return nearFilter;
}

DataType CpuTexture::getDataType() const
{
	return dataType;
}

Wrapping CpuTexture::getWrapping() const
{
	return wrapping;
}

inline void CpuTexture::checkIntegrity()
{
	if(width < 1)
	{
		loaded = false;
		logError("Image width invalid.");
		return;
	}

	if(height < 1)
	{
		loaded = false;
		logError("Image height invalid.");
		return;
	}

	loaded = true;
}

const Byte* CpuTexture::getPixels() const
{
	return static_cast<const Byte*>(pixelMemory.getData());
}

Int CpuTexture::getNumberOfChannels() const
{
	switch(format)
	{
		case WritePixelsFormat::Red:
			return 1;
		case WritePixelsFormat::RG:
			return 2;
		case WritePixelsFormat::RGB:
			return 3;
		case WritePixelsFormat::RGBA:
			return 4;
		case WritePixelsFormat::RedInteger:
			return 1;
		case WritePixelsFormat::RGInteger:
			return 2;
		case WritePixelsFormat::RGBInteger:
			return 3;
		case WritePixelsFormat::RGBAInteger:
			return 4;
		default:
			logError("Format does not have channel count!"); // [TODO] might need rewrite
	}
	return -1;
}

WritePixelsFormat CpuTexture::getFormat() const
{
	return format;
}

CpuTexture::CpuTexture(const CpuTexture& other)
{
	this->pixelMemory = other.pixelMemory;
	this->width       = other.width;
	this->height      = other.height;
	this->format      = other.format;
	this->loaded      = other.loaded;
	this->wrapping    = other.wrapping;
	this->farFilter   = other.farFilter;
	this->nearFilter  = other.nearFilter;
	this->dataType    = other.dataType;
	this->name        = other.name;
}

CpuTexture& CpuTexture::operator=(const CpuTexture& other) = default;

Float CpuTexture::getRed(const UInt x, const UInt y) const
{
	Float result = 0.0f;
	switch(dataType)
	{
		case DataType::Byte:
			result = static_cast<Float>(pixelMemory.getData()[xyToIndex(x, y, 0, 1)]) / 255.0f;
			break;
		case DataType::UInt:
			result = static_cast<Float>(*reinterpret_cast<UInt*>(&pixelMemory.getData()[xyToIndex(x, y, 0, 4)]));
			break;
		case DataType::Float:
			result = *reinterpret_cast<Float*>(&pixelMemory.getData()[xyToIndex(x, y, 0, 4)]);
			break;
		default:
			logError("Invalid data type or not supported!");
	}
	return result;
}

//
//Float CPUTexture::getBlue(const UInt x, const UInt y) const
//{
//	Float result = 0.0f;
//	switch(dataType)
//	{
//		case DataType::Byte:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 2, 1)]) / 255.0f;
//			break;
//		case DataType::UInt:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 2, 4)]);
//			break;
//		case DataType::Float:
//			result = pixels[xyToIndex(x, y, 2, 4)];
//			break;
//		default:
//			logError("Invalid data type or not supported!");
//	}
//	return result;
//}
//
//Float CPUTexture::getGreen(const UInt x, const UInt y) const
//{
//	Float result = 0.0f;
//	switch(dataType)
//	{
//		case DataType::Byte:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 1, 1)]) / 255.0f;
//			break;
//		case DataType::UInt:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 1, 4)]);
//			break;
//		case DataType::Float:
//			result = pixels[xyToIndex(x, y, 1, 4)];
//			break;
//		default:
//			logError("Invalid data type or not supported!");
//	}
//	return result;
//}
//
//Float CPUTexture::getAlpha(const UInt x, const UInt y) const
//{
//	Float result = 0.0f;
//	switch(dataType)
//	{
//		case DataType::Byte:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 3, 1)]) / 255.0f;
//			break;
//		case DataType::UInt:
//			result = static_cast<Float>(pixels[xyToIndex(x, y, 3, 4)]);
//			break;
//		case DataType::Float:
//			result = pixels[xyToIndex(x, y, 3, 4)];
//			break;
//		default:
//			logError("Invalid data type or not supported!");
//	}
//	return result;
//}
//
Float CpuTexture::getRed(const Float x, const Float y) const
{
	//convert normalized coordinates to texture space
	const Float texX = x * static_cast<Float>(width);
	const Float texY = y * static_cast<Float>(height);

	//turn float to desired indices
	const UInt aX = static_cast<UInt>(floor(texX));
	const UInt aY = static_cast<UInt>(floor(texY));
	const UInt dX = static_cast<UInt>(ceil(texX));
	const UInt dY = static_cast<UInt>(ceil(texY));
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

//
//Float CPUTexture::getBlue(const Float x, const Float y) const
//{
//	//convert normalized coordinates to texture space
//	const Float texX = x * static_cast<Float>(width);
//	const Float texY = y * static_cast<Float>(height);
//
//	//turn float to desired indices
//	const UInt aX = static_cast<UInt>(floor(texX));
//	const UInt aY = static_cast<UInt>(floor(texY));
//	const UInt dX = static_cast<UInt>(ceil(texX));
//	const UInt dY = static_cast<UInt>(ceil(texY));
//	const UInt bX = dX;
//	const UInt bY = aY;
//	const UInt cX = aX;
//	const UInt cY = dY;
//
//	//get colors of adjacent pixels
//	const Float a = getBlue(aX, aY);
//	const Float b = getBlue(bX, bY);
//	const Float c = getBlue(cX, cY);
//	const Float d = getBlue(dX, dY);
//
//	//get fraction to determine blending
//	const Float fracX = fmod(texX, 1.0f);
//	const Float fracY = fmod(texY, 1.0f);
//
//	//do linear interpolation to get the final blend
//	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
//}
//
//Float CPUTexture::getGreen(const Float x, const Float y) const
//{
//	//convert normalized coordinates to texture space
//	const Float texX = x * static_cast<Float>(width);
//	const Float texY = y * static_cast<Float>(height);
//
//	//turn float to desired indices
//	const UInt aX = static_cast<UInt>(floor(texX));
//	const UInt aY = static_cast<UInt>(floor(texY));
//	const UInt dX = static_cast<UInt>(ceil(texX));
//	const UInt dY = static_cast<UInt>(ceil(texY));
//	const UInt bX = dX;
//	const UInt bY = aY;
//	const UInt cX = aX;
//	const UInt cY = dY;
//
//	//get colors of adjacent pixels
//	const Float a = getGreen(aX, aY);
//	const Float b = getGreen(bX, bY);
//	const Float c = getGreen(cX, cY);
//	const Float d = getGreen(dX, dY);
//
//	//get fraction to determine blending
//	const Float fracX = fmod(texX, 1.0f);
//	const Float fracY = fmod(texY, 1.0f);
//
//	//do linear interpolation to get the final blend
//	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
//}
//
//Float CPUTexture::getAlpha(const Float x, const Float y) const
//{
//	//convert normalized coordinates to texture space
//	const Float texX = x * static_cast<Float>(width);
//	const Float texY = y * static_cast<Float>(height);
//
//	//turn float to desired indices
//	const UInt aX = static_cast<UInt>(floor(texX));
//	const UInt aY = static_cast<UInt>(floor(texY));
//	const UInt dX = static_cast<UInt>(ceil(texX));
//	const UInt dY = static_cast<UInt>(ceil(texY));
//	const UInt bX = dX;
//	const UInt bY = aY;
//	const UInt cX = aX;
//	const UInt cY = dY;
//
//	//get colors of adjacent pixels
//	const Float a = getAlpha(aX, aY);
//	const Float b = getAlpha(bX, bY);
//	const Float c = getAlpha(cX, cY);
//	const Float d = getAlpha(dX, dY);
//
//	//get fraction to determine blending
//	const Float fracX = fmod(texX, 1.0f);
//	const Float fracY = fmod(texY, 1.0f);
//
//	//do linear interpolation to get the final blend
//	return lerp(lerp(a, b, fracX), lerp(c, d, fracX), fracY);
//}

Index CpuTexture::xyToIndex(const Int x, const Int y, const Int channel, const Int numBytes) const
{
	// [TODO] make sure it can handle negative values
	return static_cast<UInt>(getNumberOfChannels()) * (y * width + x) + channel * numBytes;
}

void CpuTexture::load(const Path& path, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping)
{
	const Name name = getFileName(path);
	load(path, name, nearFilter, farFilter, wrapping);
}

void CpuTexture::load(const Path& path, const Name& name, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping)
{
	if(loaded) return;
	Image image;
	image.load(path);
	load(image, nearFilter, farFilter, wrapping, name);
}

void CpuTexture::load(const Image& image, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping, const Name& name)
{
	if(image.isLoaded())
	{
		this->name       = name;
		this->width      = static_cast<Int>(image.getWidth());
		this->height     = static_cast<Int>(image.getHeight());
		this->farFilter  = farFilter;
		this->nearFilter = nearFilter;
		this->wrapping   = wrapping;
		this->name       = name;

		pixelMemory = Memory(image.getDataPointer(), image.getByteSize());

		switch(image.getDataType())
		{
			case ImageDataType::Byte:
				dataType = DataType::Byte;
				switch(image.getChannels())
				{
					case Channels::Red:
						format = WritePixelsFormat::Red;
						break;
					case Channels::RG:
						format = WritePixelsFormat::RG;
						break;
					case Channels::RGB:
						format = WritePixelsFormat::RGB;
						break;
					case Channels::RGBA:
						format = WritePixelsFormat::RGBA;
						break;
				}
				loaded = true;
				break;

			case ImageDataType::Float:
				dataType = DataType::Float;
				switch(image.getChannels())
				{
					case Channels::Red:
						format = WritePixelsFormat::Red;
						break;
					case Channels::RG:
						format = WritePixelsFormat::RG;
						break;
					case Channels::RGB:
						format = WritePixelsFormat::RGB;
						break;
					case Channels::RGBA:
						format = WritePixelsFormat::RGBA;
						break;
				}
				loaded = true;
				break;
			case ImageDataType::UInt:
				dataType = DataType::UInt;
				switch(image.getChannels())
				{
					case Channels::Red:
						format = WritePixelsFormat::RedInteger;
						break;
					case Channels::RG:
						format = WritePixelsFormat::RGInteger;
						break;
					case Channels::RGB:
						format = WritePixelsFormat::RGBInteger;
						break;
					case Channels::RGBA:
						format = WritePixelsFormat::RGBAInteger;
						break;
				}
				loaded = true;
				break;
			default:
				logError("Invalid data type or channels!");
				break;
		}
	}
	else
	{
		logError("Image could not be loaded into texture!");
	}
}

void CpuTexture::load(const Name& name, const Filter nearFilter, const Filter farFilter, const Wrapping wrapping)
{
	if(!loaded)
	{
		load(resourceManager.getTextureResourcePath(name), name, nearFilter, farFilter, wrapping);
	}
	else
	{
		logWarning("Texture already loaded, you are trying to load it again, something must be wrong!");
	}
}

CpuTexture::CpuTexture(const Int width, const Int height, const Name& name, const Filter farFilter, const Filter nearFilter, const DataType dataType, const Wrapping wrapping, const WritePixelsFormat format)
{
	this->width      = width;
	this->height     = height;
	this->name       = name;
	this->farFilter  = farFilter;
	this->nearFilter = nearFilter;
	this->dataType   = dataType;
	this->wrapping   = wrapping;
	this->format     = format;
	checkIntegrity();
}
