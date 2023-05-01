#include "Image.hpp"

namespace stbi {
	//#define STBI_NO_JPEG
#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
}

void Image::load(Path path, Bool inverted) {
	//make sure the file exists
	if (!doesPathExist(path)) {
		logError(String("File not found! (").append(path.string()).append(")"));
	}

	//get the bitcount of the image
	bitcount = 8;
	if (path.extension() == ".hdr" || path.extension() == ".exr") bitcount = 32;

	//load the image
	stbi::stbi_set_flip_vertically_on_load(inverted);
	void* pixelData = nullptr;
	switch (bitcount) {
		case 8:  pixelData = stbi::stbi_load(path.string().c_str(), &width, &height, &channels, 0);  break;
		case 32: pixelData = stbi::stbi_loadf(path.string().c_str(), &width, &height, &channels, 0); break;
		default: logError("Bitcount not supported!"); break;
	}

	//check if the image was loaded
	if (pixelData == nullptr) {
		logError(String("Failed to load image! (").append(path.string()).append(")"));
	}

	//copy the pixel data
	pixels.resize(width * height * channels * bitcount / 8);
	std::memcpy(pixels.data(), pixelData, pixels.size());

	//free the pixel data
	stbi::stbi_image_free(pixelData);

	logDebug(String("Loaded image: (").append(path.string()).append(")"));
}