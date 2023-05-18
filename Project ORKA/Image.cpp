#include "Image.hpp"

namespace stbi {
	//#define STBI_NO_JPEG
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

#include "webp/decode.h"
#include "webp/types.h"

void Image::advancedWebPLoader(const Byte* fileData, const ULL fileSize) {
	//WebP can load at desired resolution
	//WebP can load with desired cropping
	//WebP supports alpha
	//WebP supports animation
	//WebP supports multithreaded decoding

	//create a WebPDecoderConfig object
	WebPDecoderConfig config;
	if (WebPInitDecoderConfig(&config) != 1) {
		logError("Could not initialize WebP Decoder Config!");
		loaded = false;
		return;
	}

	// set decoding options
	config.options.flip = 1;
	config.options.use_threads = 1;

	//get the WebP info
	if (WebPGetFeatures(fileData, fileSize, &config.input) != VP8_STATUS_OK) {
		logError("Could not get WebP Information!");
		loaded = false;
		return;
	}

	if (config.input.has_alpha) config.output.colorspace = MODE_RGBA;
	else config.output.colorspace = MODE_RGB;

	const auto result = WebPDecode(fileData, fileSize, &config);
	if (result == VP8_STATUS_OK) {
		//set channels
		channels = 0;
		if (config.output.colorspace == MODE_RGB) channels = 3;
		if (config.output.colorspace == MODE_RGBA) channels = 4;
		if (channels == 0) {
			logError("Incorrect colorspace!");
			WebPFreeDecBuffer(&config.output);
			loaded = false;
			return;
		}

		//set width
		width = config.output.width;

		//set height
		height = config.output.height;

		//set bitCount
		bitCount = 8;

		//set pixels
		const ULL imageSize = width * height * channels;
		pixels.resize(imageSize);
		std::copy(config.output.u.RGBA.rgba, config.output.u.RGBA.rgba + imageSize, pixels.begin());
		WebPFreeDecBuffer(&config.output);

		//set loaded
		loaded = true;
	}
	else {
		logError("Could not decode WebP");
		loaded = false;
		return;
	}
}

void Image::loadWebP(const Path& path) {
	//[TODO] what about animations?

	// Read the WebP file
	FILE* file = fopen(path.string().c_str(), "rb");
	if (!file) {
		logError(String("Could not open file: ").append(path.string()));
		loaded = false;
		return;
	}

	// Determine the file size
	fseek(file, 0, SEEK_END);
	const size_t fileSize = ftell(file);
	rewind(file);

	// Allocate memory for the file data
	auto fileData = new Byte[fileSize];

	// Read the file data into memory
	const size_t bytesRead = fread(fileData, 1, fileSize, file);
	if (bytesRead != fileSize) {
		logError(String("Could not open file: ").append(path.string()));
		delete[] fileData;
		fclose(file);
		return;
	}

	fclose(file);

	advancedWebPLoader(fileData, fileSize);

	delete[] fileData;
};

void Image::load(const Path& path, const Bool inverted) {
	if (path.extension() == ".webp") {
		loadWebP(path);
		return;
	}

	//make sure the file exists
	if (!doesPathExist(path)) { logError(String("File not found! (").append(path.string()).append(")")); }

	//get the bitCount of the image
	bitCount = 8;
	if (path.extension() == ".hdr" || path.extension() == ".exr") bitCount = 32;

	//load the image
	stbi::stbi_set_flip_vertically_on_load(inverted);
	Byte* pixelData = nullptr;
	switch (bitCount) {
		case 8: pixelData = stbi::stbi_load(path.string().c_str(), &width, &height, &channels, 0);
			break;
		case 32: pixelData = reinterpret_cast<Byte*>(stbi::stbi_loadf(path.string().c_str(), &width, &height, &channels,
		                                                              0));
			break;
		default: logError("Bitcount not supported!");
			loaded = false;
			return;
	}

	//check if the image was loaded
	if (pixelData == nullptr) {
		logError(String("Failed to load image! (").append(path.string()).append(")"));
		loaded = false;
		return;
	}
	//copy the pixel data
	const UInt byteSize = width * height * channels * bitCount / 8;
	pixels.resize(byteSize);
	std::copy(pixelData, pixelData + byteSize, pixels.data());

	//free the pixel data
	stbi::stbi_image_free(pixelData);

	logDebug(String("Loaded image: (").append(path.string()).append(")"));
}
