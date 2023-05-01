
#pragma once

#include "GraphicsAPI.hpp"
#include "ResourceManager.hpp"

enum class DataType : UInt {
	dataTypeByte = GL_UNSIGNED_BYTE,
	dataTypeFloat = GL_FLOAT,
	dataTypeUInt = GL_UNSIGNED_INT
};
enum class Wrapping : Int
{
	repeat = GL_REPEAT,
	mirrored = GL_MIRRORED_REPEAT,
	clamped = GL_CLAMP_TO_EDGE,
	border = GL_CLAMP_TO_BORDER
};
enum class Filter : Int {
	nearest = GL_NEAREST,
	nearestMM = GL_NEAREST_MIPMAP_NEAREST,
	linear = GL_LINEAR,
	linearMM = GL_LINEAR_MIPMAP_LINEAR
};

struct CPUTexture {
	Int width = 0;
	Int height = 0;
	Int channels = 0;
	Name name = "NULL";
	Bool loaded = false;
	Filter farFilter = Filter::linear;
	Filter nearFilter = Filter::linear;
	Wrapping wrapping = Wrapping::repeat;
	DataType dataType = DataType::dataTypeByte;

	Vector<Byte> bytePixels;
	Vector<Float> floatPixels;
	Vector<UInt> uIntPixels;

	Float getRed(UInt x, UInt y);
	Float getGreen(UInt x, UInt y);
	Float getBlue(UInt x, UInt y);
	Float getAlpha(UInt x, UInt y);

	Float getRed(Float x, Float y);
	Float getGreen(Float x, Float y);
	Float getBlue(Float x, Float y);
	Float getAlpha(Float x, Float y);

	void unload();
	void load(Path path);
	void load(Path path, Name name);
	void load(ResourceManager& resourceManager, Name name);
	void load(Image image, Filter nearFilter, Filter farFilter, Wrapping wrapping, Name name);

	Index xyToIndex(Int x, Int y, Int channel);

	CPUTexture();
	~CPUTexture();
	CPUTexture(const CPUTexture& other);
	CPUTexture& operator=(const CPUTexture& other);
};