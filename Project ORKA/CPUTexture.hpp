#pragma once

#ifdef DEBUG_TEXTURES
#define DEBUG
#else
#undef DEBUG
#endif

#include "Basics.hpp"
#include "FileSystem.hpp"
#include "Debug.hpp"
#include "GraphicsAPI.hpp"
#include "Math.hpp"

enum DataType {
	dataTypeByte = GL_UNSIGNED_BYTE,
	dataTypeFloat = GL_FLOAT,
	dataTypeUInt = GL_UNSIGNED_INT
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
	DataType dataType = dataTypeByte;
	union {
		void* pixels = nullptr;
		Byte* bytePixels;
		Float* floatPixels;
		UInt* uIntPixels;
	};
	Filter nearFilter = Filter::linear;
	Filter farFilter = Filter::linear;
	Int wrapping = repeat;
	Bool loaded = false;

	Float getRed(UInt x, UInt y);
	Float getGreen(UInt x, UInt y);
	Float getBlue(UInt x, UInt y);
	Float getRed(Float x, Float y);
	Float getAlpha(UInt x, UInt y);
	Float getGreen(Float x, Float y);
	Float getBlue(Float x, Float y);
	Float getAlpha(Float x, Float y);

	void unload();
	void load(Name name);
	void load(Path path, Name name);

	Index xyToIndex(Int x, Int y, Int channel);

	CPUTexture() {};
	~CPUTexture();
	CPUTexture(CPUTexture&&) = delete;
	CPUTexture(const CPUTexture&) = delete;
	CPUTexture& operator=(CPUTexture&&) = delete;
	CPUTexture& operator=(const CPUTexture&) = delete;
};