
#include "FileSystem.hpp"

namespace stbi {
	#define STBI_NO_JPEG
	#define STBI_FAILURE_USERMSG
	#define STB_IMAGE_IMPLEMENTATION
	#include "stb_image.h"
}

String loadString(Path path) {
	String s;
	std::ifstream stream(path, std::ios::in);
	if(!stream.is_open()) logError(String("String Cant be read! (").append(path.string()).append(")"));
	std::stringstream sstr;
	sstr << stream.rdbuf();
	s = sstr.str();
	stream.close();
	return s;
}

Image loadImage(Path path, Int bitcount, Bool inverted) {
	stbi::stbi_set_flip_vertically_on_load(inverted);

	Image image;
	if (bitcount == 8) {
		image.pixels = stbi::stbi_load(path.string().c_str(), &image.width, &image.height, &image.channels, 0);
	}
	else {
		if (bitcount == 16) {
			image.pixels = reinterpret_cast <Byte*>(stbi::stbi_load_16(path.string().c_str(), &image.width, &image.height, &image.channels, 0));
			image.bitcount = 16;
		}
		else {
			logError("Bitcount not supported!");
		}
	}
	return image;
}