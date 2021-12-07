#pragma once

#include "Basics.hpp"
#include "Debug.hpp"
#include "GraphicsAPI.hpp"

enum DataType {
	dataTypeByte = GL_UNSIGNED_BYTE,
	dataTypeFloat = GL_FLOAT
};
enum Wrapping
{
	repeat = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	clamped = GL_CLAMP_TO_EDGE,
	border = GL_CLAMP_TO_BORDER
};
enum class Filter {
	nearest = GL_NEAREST,
	nearestMM = GL_NEAREST_MIPMAP_NEAREST,
	linear = GL_LINEAR,
	linearMM = GL_LINEAR_MIPMAP_LINEAR
};

struct CPUTexture {
	Name name = "NULL";
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Int dataType = dataTypeByte;
	union {
		void* pixels = nullptr;
		Byte* bytePixels;
		Float* floatPixels;
	};
	Filter nearFilter = Filter::linear;
	Filter farFilter = Filter::linearMM;
	Int wrapping = repeat;
	Bool loaded = false;

	void unload();
	void load(Path path, Name name);
	void load(Name name);;

	CPUTexture() {};
	~CPUTexture();
	CPUTexture(CPUTexture&&) = delete;
	CPUTexture(const CPUTexture&) = delete;
	CPUTexture& operator=(CPUTexture&&) = delete;
	CPUTexture& operator=(const CPUTexture&) = delete;
};