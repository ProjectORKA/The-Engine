#include "Image.hpp"
#include "Debug.hpp"
#include "FileSystem.hpp"

#include "lodepng.h"

namespace External {
	#include "webp/decode.h"
	#include "jpeglib.h"
	#include "gif_lib.h"
	#define STBI_WINDOWS_UTF8
	#define STBI_FAILURE_USERMSG
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
}

using ImageHeader       = Vector<Byte>;
Int imageHeaderReadSize = 10;

void jpegErrorCallback(const External::j_common_ptr info)
{
	Char buffer[JMSG_LENGTH_MAX];
	(*info->err->format_message)(info, buffer);
	logError(buffer);
	longjmp(*static_cast<jmp_buf*>(info->client_data), 1);
}

Bool isPNG(const ImageHeader& header)
{
	return header.size() >= 8 && header[0] == 0x89 && header[1] == 0x50 && header[2] == 0x4E && header[3] == 0x47 && header[4] == 0x0D && header[5] == 0x0A && header[6] == 0x1A && header[7] == 0x0A;
}

Bool isBMP(const ImageHeader& header)
{
	return header.size() >= 2 && header[0] == 0x42 && header[1] == 0x4D; // BM
}

Bool isHDR(const ImageHeader& header)
{
	return header.size() >= 10 && header[0] == '#' && header[1] == '?' && header[2] == 'R' && header[3] == 'A' && header[4] == 'D' && header[5] == 'I' && header[6] == 'A' && header[7] == 'N' && header[8] == 'C' && header[9] == 'E';
}

Bool isEXR(const ImageHeader& header)
{
	return header.size() >= 4 && header[0] == 0x76 && header[1] == 0x2F && header[2] == 0x31 && header[3] == 0x01;
}

Bool isGIF(const ImageHeader& header)
{
	return header.size() >= 6 && header[0] == 0x47 && header[1] == 0x49 && header[2] == 0x46 && header[3] == 0x38 && (header[4] == 0x37 || header[4] == 0x39) && header[5] == 0x61;
}

Bool isJPEG(const ImageHeader& header)
{
	return header.size() >= 2 && header[0] == 0xFF && header[1] == 0xD8;
}

Bool isWebP(const ImageHeader& header)
{
	return header.size() >= 4 && header[0] == 0x52 && header[1] == 0x49 && header[2] == 0x46 && header[3] == 0x46;
}

Bool isJFIF(const ImageHeader& header)
{
	return header.size() >= 2 && header[0] == 0xFF && header[1] == 0xD8; // JPEG magic numbers
}

Bool isTIFF(const ImageHeader& header)
{
	return (header.size() >= 4 && (header[0] == 0x49 && header[1] == 0x49 && header[2] == 0x2A && header[3] == 0x00)) || // Little-endian TIFF
		(header.size() >= 4 && (header[0] == 0x4D && header[1] == 0x4D && header[2] == 0x00 && header[3] == 0x2A));      // Big-endian TIFF
}

void Image::load(const Path& path)
{
	if (debugImageLoadingIsEnabled) logDebug("Loading (" + path.string() + ")");
	if (!doesPathExist(path)) logError("File not found! (" + path.string() + ")");

	std::ifstream file(path, std::ios::binary);
	ImageHeader   header(imageHeaderReadSize);
	file.read(reinterpret_cast<char*>(header.data()), imageHeaderReadSize);

	if (isJPEG(header))
	{
		loadJPEG(path);
		if (pixelMemory.isValid()) return;
	}
	if (isWebP(header))
	{
		loadWebP(path);
		if (pixelMemory.isValid()) return;
	}
	if (isPNG(header))
	{
		loadPNG(path);
		if (pixelMemory.isValid()) return;
	}
	if (isBMP(header))
	{
		loadBMP(path);
		if (pixelMemory.isValid()) return;
	}
	if (isEXR(header))
	{
		loadEXR(path);
		if (pixelMemory.isValid()) return;
	}
	if (isGIF(header))
	{
		loadGIF(path, 0);
		if (pixelMemory.isValid()) return;
	}
	if (isHDR(header))
	{
		loadHDR(path);
		if (pixelMemory.isValid()) return;
	}
	if (isJFIF(header))
	{
		loadJFIF(path);
		if (pixelMemory.isValid()) return;
	}
	if (isTIFF(header))
	{
		loadTIFF(path);
		if (pixelMemory.isValid()) return;
	}

	throw std::runtime_error("File type not supported!\n(" + toString(path) + ")");
}

void Image::loadPNG(const Path& path)
{
	Vector<Byte> pngData;
	UInt         imageWidth, imageHeight;
	const UInt   errorCode = lodepng::decode(pngData, imageWidth, imageHeight, path.string());

	if (errorCode != 0)
	{
		logWarning("Library lodepng failed to load file: " + toString(lodepng_error_text(errorCode)) + "\n");
		return;
	}

	pixelMemory = Memory(pngData.data(), pngData.size());
	width       = static_cast<Int>(imageWidth);
	height      = static_cast<Int>(imageHeight);
	loaded      = true;

	flipVertically();
}

void Image::loadWebP(const Path& path)
{
	// [TODO] what about animations?

	// open the webp file
	FILE* file = fopen(path.string().c_str(), "rb");
	if (!file)
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
	if (bytesRead != fileSize)
	{
		logError("Could not open file: " + path.string());
		delete[] fileData;
		fclose(file);
		return;
	}

	// close the file
	fclose(file);

	// create a WebPDecoderConfig object
	External::WebPDecoderConfig config;
	if (WebPInitDecoderConfig(&config) != 1)
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
	if (WebPGetFeatures(fileData, fileSize, &config.input) != External::VP8_STATUS_OK)
	{
		logWarning("Could not get WebP Information!\n File: (" + path.string() + ")");
		loaded = false;
		return;
	}

	// handle animation
	if (config.input.has_animation)
	{
		loaded = false;
		delete[] fileData;
		logWarning("Cant handle animated images!");
		return;
	}

	// handle alpha
	if (config.input.has_alpha)
	{
		config.output.colorspace = External::MODE_RGBA;
	}
	else
	{
		config.output.colorspace = External::MODE_RGB;
	}

	// decode
	if (const auto result = WebPDecode(fileData, fileSize, &config); result == External::VP8_STATUS_OK)
	{
		// set channels
		channels = Channels::Red;
		if (config.output.colorspace == External::MODE_RGB) channels = Channels::RGB;
		if (config.output.colorspace == External::MODE_RGBA) channels = Channels::RGBA;
		if (channels == Channels::Red)
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

void Image::flipVertically() const
{
	if (!loaded)
	{
		logError("Image is not loaded.");
		return;
	}

	Vector<Byte> tempRow(width * getChannelCount());
	const Int    rowSize = width * getChannelCount();

	for (Int i = 0; i < height / 2; ++i)
	{
		std::copy(pixelMemory.getData() + i * rowSize, pixelMemory.getData() + (i + 1) * rowSize, tempRow.begin());
		std::copy(pixelMemory.getData() + (height - 1 - i) * rowSize, pixelMemory.getData() + (height - i) * rowSize, pixelMemory.getData() + i * rowSize);
		std::copy(tempRow.begin(), tempRow.end(), pixelMemory.getData() + (height - 1 - i) * rowSize);
	}
}

Image::Image(const Int width, const Int height, const Channels channels, const ImageDataType dataType, const Memory& pixels)
{
	create(width, height, channels, 1, dataType, pixels);
}

void Image::loadBMP(const Path& path)
{
	loadOther(path, true);
}

void Image::loadEXR(const Path& path)
{
	dataType = ImageDataType::Float;
	loadOther(path, true);
}

void Image::loadGIF(const Path& path, const Int frameId)
{
	const String           stringPath = toString(path);
	const char*            charPath   = stringPath.c_str();
	External::GifFileType* gif        = External::DGifOpenFileName(charPath, nullptr);
	if (gif == nullptr)
	{
		logError("Could not load gif!");
		return;
	}

	if (DGifSlurp(gif) != 1)
	{
		logError("Error slurping Gif File!");
		DGifCloseFile(gif, nullptr);
		return;
	}

	printf("Number of frames: %d\n", gif->ImageCount);

	width               = gif->SWidth;
	height              = gif->SHeight;
	channels            = Channels::RGB;
	dataType            = ImageDataType::Byte;
	const Int numPixels = width * height * 3;
	pixelMemory         = Memory(numPixels);

	unsigned char* pixelPtr = pixelMemory.getData();

	const External::SavedImage*   frame     = &gif->SavedImages[frameId];
	const External::GifImageDesc* frameDesc = &frame->ImageDesc;
	const External::GifByteType*  raster    = frame->RasterBits;

	const External::GifColorType* colorMap = nullptr;
	if (gif->SColorMap) colorMap = gif->SColorMap->Colors;
	if (gif->SavedImages[frameId].ImageDesc.ColorMap) colorMap = gif->SavedImages[frameId].ImageDesc.ColorMap->Colors;

	if (!colorMap)
	{
		logError("Color Map for gif not found!");
		pixelMemory.destroy();
		return;
	}

	for (Int y = height - 1; y >= 0; y--)
	{
		for (Int x = 0; x < width; x++)
		{
			const Int                    index      = y * width + x;
			const Int                    colorIndex = raster[index];
			const External::GifColorType color      = colorMap[colorIndex];

			*pixelPtr++ = color.Red;
			*pixelPtr++ = color.Green;
			*pixelPtr++ = color.Blue;
		}
	}

	loaded = true;

	DGifCloseFile(gif, nullptr);
}

void Image::loadHDR(const Path& path)
{
	dataType = ImageDataType::Float;
	loadOther(path, true);
}

void Image::loadJPEG(const Path& path)
{
	FILE* inFile = External::stbi__fopen(toString(path).c_str(), "rb");

	if (!inFile)
	{
		logError("Error opening input file.");
		return;
	}

	External::jpeg_decompress_struct info;
	External::jpeg_error_mgr         error;
	info.err         = jpeg_std_error(&error);
	error.error_exit = jpegErrorCallback;

	//if (setjmp(jerr.setjmp_buffer)) {
	//    jpeg_destroy_decompress(&cinfo);
	//    fclose(infile);
	//    return 1;
	//}

	jpeg_CreateDecompress((&info), 62, (size_t)sizeof(struct External::jpeg_decompress_struct));
	jpeg_stdio_src(&info, inFile);
	jpeg_read_header(&info, 1);
	jpeg_start_decompress(&info);

	width           = info.output_width;
	height          = info.output_height;
	Int numChannels = info.output_components;

	switch (info.output_components)
	{
	case 1: channels = Channels::Red;
		break;
	case 2: channels = Channels::RG;
		break;
	case 3: channels = Channels::RGB;
		break;
	case 4: channels = Channels::RGBA;
		break;
	default: logError("Image (" + getFileName(path) + ") did not load with correct channels!");
	}

	unsigned long  image_size = width * height * numChannels;
	unsigned char* buffer     = new unsigned char[image_size];

	while (info.output_scanline < height)
	{
		Byte* rowPointer = &buffer[info.output_scanline * width * numChannels];
		jpeg_read_scanlines(&info, &rowPointer, 1);
	}

	pixelMemory = Memory(buffer, static_cast<ULL>(width * height * numChannels));

	jpeg_finish_decompress(&info);
	jpeg_destroy_decompress(&info);

	if (Int error = fclose(inFile) != 0)
	{
		logWarning("Error closing JPEG!");
	};

	delete[] buffer;

	if (!pixelMemory.isValid())
	{
		logError("Failed to load image! (" + toString(path) + ")");
		loaded = false;
		delete[] buffer;
		return;
	}

	loaded = true;

	flipVertically();
}

void Image::loadJFIF(const Path& path)
{
	loadOther(path, true);
}

void Image::loadTIFF(const Path& path)
{
	loadOther(path, true);
}

Image::Image() = default;

Bool Image::isLoaded() const
{
	return loaded;
}

UInt Image::getWidth() const
{
	if (!loaded) logError("Cant read property from unloaded image!");
	return width;
}

UInt Image::getHeight() const
{
	if (!loaded) logError("Cant read property from unloaded image!");
	return height;
}

SizeT Image::getByteSize() const
{
	if (!loaded) logError("Cant read property from unloaded image!");

	switch (getDataType())
	{
	case ImageDataType::Byte: return width * height * getChannelCount();
		break;
	case ImageDataType::Float:
	case ImageDataType::UInt:
	case ImageDataType::Int: return width * height * getChannelCount() * 4;
		break;
	default: ;
		return 0;
	}
}

Channels Image::getChannels() const
{
	if (!loaded) logError("Cant read property from unloaded image!");
	return channels;
}

Byte* Image::getDataPointer() const
{
	if (!loaded) logError("Cant read property from unloaded image!");
	return static_cast<Byte*>(pixelMemory.getData());
}

Int Image::getChannelCount() const
{
	switch (channels)
	{
	case Channels::Red: return 1;
	case Channels::RG: return 2;
	case Channels::RGB: return 3;
	case Channels::RGBA: return 4;
	default: logError("Invalid channel count!");
		return 0;
	}
}

ImageDataType Image::getDataType() const
{
	if (!loaded) logError("Cant read property from unloaded image!");
	return dataType;
}

void Image::loadOther(const Path& path, const Bool inverted)
{
	External::stbi_set_flip_vertically_on_load(inverted);

	Int   channelsInt = 0;
	Byte* address     = nullptr;
	SizeT byteSize    = 0;

	const String filePath = toString(path);

	switch (dataType)
	{
	case ImageDataType::Byte: address = External::stbi_load(filePath.c_str(), &width, &height, &channelsInt, External::STBI_rgb_alpha);
		byteSize = static_cast<SizeT>(width) * static_cast<SizeT>(height) * 4 * sizeof(Byte); // Force RGBA, 4 channels
		break;
	case ImageDataType::Float: address = reinterpret_cast<Byte*>(External::stbi_loadf(filePath.c_str(), &width, &height, &channelsInt, External::STBI_rgb_alpha));
		byteSize = static_cast<SizeT>(width) * static_cast<SizeT>(height) * 4 * sizeof(Float); // Force RGBA, 4 channels
		break;
	default: logError("Data type not supported!");
		loaded = false;
		return;
	}

	pixelMemory = Memory(address, byteSize);

	External::stbi_image_free(address);

	channels = Channels::RGBA; // Force RGBA

	if (!pixelMemory.isValid())
	{
		logError("Failed to load image! (" + toString(path) + ")");
		loaded = false;
		return;
	}

	loaded = true;
}

void Image::create(const Int width, const Int height, const Channels channels, const Int frames, const ImageDataType dataType, const Memory& pixels)
{
	// float int and uint all have 4 bytes, so this is a small optimization
	SizeT byteCount = 4;
	if (dataType == ImageDataType::Byte) byteCount = 1;

	if (pixels.getSize() == static_cast<SizeT>(width * height * static_cast<Int>(channels)) * byteCount)
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
