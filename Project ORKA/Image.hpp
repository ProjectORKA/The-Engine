
#pragma once

#include "Basics.hpp"
#include "FileSystem.hpp"

struct Image {
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int bitcount = 8;
	Vector<Byte> pixels;

	void load(Path path, Bool inverted);
};