#include "Image.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "lodepng.h"

namespace stbi{
#define STBI_WINDOWS_UTF8
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

#include "webp/decode.h"

Image::Image() = default;

Bool Image::isLoaded() const
{
	return loaded;
}

UInt Image::getWidth() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return width;
}

UInt Image::getHeight() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return height;
}

SizeT Image::getByteSize() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return pixelMemory.getSize();
}

void Image::load(const Path& path)
{
	if(debugImageLoadingIsEnabled) logDebug("Loading (" + path.string() + ")");

	// make sure file exists
	if(!doesPathExist(path)) logError("File not found! (" + path.string() + ")");

	const String fileType = getFileExtension(path);

	if(fileType == ".webp")
	{
		loadWebP(path);
		if(pixelMemory.isValid()) return;
	}

	if(fileType == ".hdr" || fileType == ".exr")
	{
		dataType = ImageDataType::Float;
		loadOther(path, true);
		if(pixelMemory.isValid()) return;
	}

	if(fileType == ".bmp")
	{
		loadOther(path, true);
		if(pixelMemory.isValid()) return;
	}

	if(fileType == ".png")
	{
		loadPNG(path);
		if(pixelMemory.isValid()) return;
	}

	if(fileType == ".jpg" || fileType == ".jpeg" || fileType == ".jfif" || fileType == ".png")
	{
		loadOther(path, true);
		return;
	}

	logError("File type not supported!");
}

Channels Image::getChannels() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return channels;
}

Byte* Image::getDataPointer() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return static_cast<Byte*>(pixelMemory.getData());
}

void Image::loadPNG(const Path& path)
{
	Vector<Byte> pngData;
	UInt         imageWidth, imageHeight;
	const UInt   errorCode = lodepng::decode(pngData, imageWidth, imageHeight, path.string());

	if(errorCode != 0)
	{
		logWarning("Library lodepng failed to load file: " + toString(lodepng_error_text(errorCode)) + "\n");
		return;
	}

	// Flip the image vertically
	const UInt  bytesPerPixel = 4; // Assuming RGBA channels
	const SizeT rowSize       = imageWidth * bytesPerPixel;
	const SizeT imageSize     = rowSize * imageHeight;

	Vector<Byte> flippedData(imageSize);
	for(UInt y = 0; y < imageHeight; ++y)
	{
		const SizeT srcOffset = y * rowSize;
		const SizeT dstOffset = (imageHeight - y - 1) * rowSize;
		std::memcpy(flippedData.data() + dstOffset, pngData.data() + srcOffset, rowSize);
	}

	pixelMemory = Memory(flippedData.data(), flippedData.size());
	width       = static_cast<Int>(imageWidth);
	height      = static_cast<Int>(imageHeight);
	loaded      = true;
}

void Image::loadWebP(const Path& path)
{
	// [TODO] what about animations?

	// open the webp file
	FILE* file = fopen(path.string().c_str(), "rb");
	if(!file)
	{
		logError("Could not open file: " + path.string());
		loaded = false;
		return;
	}

	// determine the file size
	fseek(file, 0, SEEK_END);
	const size_t fileSize = ftell(file);
	rewind(file);

	// allocate memory for the file data
	const auto fileData = new Byte[fileSize];

	// Read the file data into memory
	const size_t bytesRead = fread(fileData, 1, fileSize, file);
	if(bytesRead != fileSize)
	{
		logError("Could not open file: " + path.string());
		delete[] fileData;
		fclose(file);
		return;
	}

	// close the file
	fclose(file);

	// create a WebPDecoderConfig object
	WebPDecoderConfig config;
	if(WebPInitDecoderConfig(&config) != 1)
	{
		logWarning("Could not initialize WebP Decoder Config!\n File: (" + path.string() + ")");
		loaded = false;
		delete[] fileData;
		return;
	}

	// set decoding options
	config.options.flip        = 1;
	config.options.use_threads = 1;

	// get the WebP info
	if(WebPGetFeatures(fileData, fileSize, &config.input) != VP8_STATUS_OK)
	{
		logWarning("Could not get WebP Information!\n File: (" + path.string() + ")");
		loaded = false;
		return;
	}

	// handle animation
	if(config.input.has_animation)
	{
		loaded = false;
		delete[] fileData;
		logWarning("Cant handle animated images!");
		return;
	}

	// handle alpha
	if(config.input.has_alpha) config.output.colorspace = MODE_RGBA;
	else config.output.colorspace                       = MODE_RGB;

	// decode
	if(const auto result = WebPDecode(fileData, fileSize, &config); result == VP8_STATUS_OK)
	{
		// set channels
		channels = Channels::Red;
		if(config.output.colorspace == MODE_RGB) channels = Channels::RGB;
		if(config.output.colorspace == MODE_RGBA) channels = Channels::RGBA;
		if(channels == Channels::Red)
		{
			WebPFreeDecBuffer(&config.output);
			loaded = false;
			logError("Incorrect color space!");
		}

		// set width
		width = config.output.width;

		// set height
		height = config.output.height;

		// set data type
		dataType = ImageDataType::Byte;

		// set pixels
		const ULL imageSize = static_cast<ULL>(width) * static_cast<ULL>(height) * static_cast<ULL>(channels);
		pixelMemory         = Memory(config.output.u.RGBA.rgba, imageSize);

		WebPFreeDecBuffer(&config.output);

		// set loaded
		loaded = true;
	}
	else
	{
		logError("Could not decode WebP");
		loaded = false;
	}

	delete[] fileData;
}

ImageDataType Image::getDataType() const
{
	if(!loaded) logError("Cant read property from unloaded image!");
	return dataType;
}

void Image::loadOther(const Path& path, const Bool inverted)
{
	stbi::stbi_set_flip_vertically_on_load(inverted);

	Int   channelsInt = 0;
	Byte* address     = nullptr;
	SizeT byteSize    = 0;

	const String filePath = toString(path);

	switch(dataType)
	{
		case ImageDataType::Byte:
			address = stbi::stbi_load(filePath.c_str(), &width, &height, &channelsInt, stbi::STBI_rgb_alpha);
			byteSize = static_cast<SizeT>(width) * static_cast<SizeT>(height) * 4 * sizeof(Byte); // Force RGBA, 4 channels
			break;
		case ImageDataType::Float:
			address = reinterpret_cast<Byte*>(stbi::stbi_loadf(filePath.c_str(), &width, &height, &channelsInt, stbi::STBI_rgb_alpha));
			byteSize = static_cast<SizeT>(width) * static_cast<SizeT>(height) * 4 * sizeof(Float); // Force RGBA, 4 channels
			break;
		default:
			logError("Data type not supported!");
			loaded = false;
			return;
	}

	pixelMemory = Memory(address, byteSize);

	stbi::stbi_image_free(address);

	channels = Channels::RGBA; // Force RGBA

	if(!pixelMemory.isValid())
	{
		logError("Failed to load image! (" + toString(path) + ")");
		loaded = false;
		return;
	}

	loaded = true;
}

Image::Image(const Int width, const Int height, const Channels channels, const ImageDataType dataType, const Memory& pixels)
{
	// float int and uint all have 4 bytes, so this is a small optimization
	SizeT byteCount = 4;
	if(dataType == ImageDataType::Byte) byteCount = 1;

	if(pixels.getSize() == static_cast<SizeT>(width * height * static_cast<Int>(channels)) * byteCount)
	{
		this->width       = width;
		this->height      = height;
		this->channels    = channels;
		this->dataType    = dataType;
		this->pixelMemory = pixels;
		this->loaded      = true;
		return;
	}
	logError("Invalid parameters. Cant create image!");
}
