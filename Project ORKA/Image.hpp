#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

#include "WebP/decode.h"

struct Image {
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int bitCount = 8;
	Bool loaded = false;
	Vector<Byte> pixels;

	void loadWebP(const Path& path);
	void load(const Path& path, Bool inverted);
	void advancedWebPLoader(const Byte* fileData, ULL fileSize);
};
