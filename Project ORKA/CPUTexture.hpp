#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "stb_image.h"
#include "GraphicsAPI.hpp"

enum DataType {
#ifdef GRAPHICS_API_OPENGL
	dataTypeByte = GL_UNSIGNED_BYTE,
	dataTypeFloat = GL_FLOAT
#endif
};
enum Wrapping
{
#ifdef GRAPHICS_API_OPENGL
	repeat = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	clamped = GL_CLAMP_TO_EDGE,
	border = GL_CLAMP_TO_BORDER
#endif
};
enum Filter {
#ifdef GRAPHICS_API_OPENGL
	nearest = GL_NEAREST,
	nearestMM = GL_NEAREST_MIPMAP_NEAREST,
	linear = GL_LINEAR,
	linearMM = GL_LINEAR_MIPMAP_LINEAR
#endif
};

struct CPUTexture {
	Name name = "empty";
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int dataType = dataTypeByte;
	union {
		void* pixels;
		Byte* bytePixels;
		Float* floatPixels;
	};
	Int filter = linearMM;
	Int wrapping = repeat;
	Bool loaded = false;

	void load(Path path, Name name);
	void loadHDR(Path path, Name name);
	void unload();

	~CPUTexture();
};