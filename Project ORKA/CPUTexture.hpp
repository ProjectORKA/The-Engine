#pragma once
#include "Image.hpp"
#include "GraphicsAPI.hpp"

struct CPUTexture
{
	CPUTexture& operator=(const CPUTexture& other);

	~CPUTexture();
	CPUTexture() = default;
	CPUTexture(const CPUTexture& other);
	CPUTexture(Int width, Int height, const Name& name, Filter farFilter, Filter nearFilter, DataType dataType, Wrapping wrapping, WritePixelsFormat format);

	[[nodiscard]] Name              getName() const;
	[[nodiscard]] Int               getWidth() const;
	[[nodiscard]] Bool              isLoaded() const;
	[[nodiscard]] Int               getHeight() const;
	[[nodiscard]] const Byte*       getPixels() const;
	[[nodiscard]] WritePixelsFormat getFormat() const;
	[[nodiscard]] DataType          getDataType() const;
	[[nodiscard]] Wrapping          getWrapping() const;
	[[nodiscard]] Filter            getFarFilter() const;
	[[nodiscard]] Filter            getNearFilter() const;
	[[nodiscard]] Float             getAspectRatio() const;
	[[nodiscard]] Int               getNumberOfChannels() const;

	[[nodiscard]] Float getRed(UInt x, UInt y) const;
	// [[nodiscard]] Float             getBlue(UInt x, UInt y) const;
	// [[nodiscard]] Float             getGreen(UInt x, UInt y) const;
	// [[nodiscard]] Float             getAlpha(UInt x, UInt y) const;

	[[nodiscard]] Float getRed(Float x, Float y) const;
	// [[nodiscard]] Float             getBlue(Float x, Float y) const;
	// [[nodiscard]] Float             getGreen(Float x, Float y) const;
	// [[nodiscard]] Float             getAlpha(Float x, Float y) const;

	[[nodiscard]] Index xyToIndex(Int x, Int y, Int channel, Int numBytes) const;

	void unload();
	void checkIntegrity();
	void load(const Path& path, Filter nearFilter, Filter farFilter, Wrapping wrapping);
	void load(const Name& name, Filter nearFilter, Filter farFilter, Wrapping wrapping);
	void load(const Path& path, const Name& name, Filter nearFilter, Filter farFilter, Wrapping wrapping);
	void load(const Image& image, Filter nearFilter, Filter farFilter, Wrapping wrapping, const Name& name);
private:
	Memory            pixelMemory;
	Int               width      = 0; // needs to be int for OpenGL and error correction
	Int               height     = 0; // needs to be int for OpenGL and error correction
	Name              name       = "NULL";
	Bool              loaded     = false;
	Filter            farFilter  = Filter::Linear;
	Filter            nearFilter = Filter::Linear;
	DataType          dataType   = DataType::Byte;
	Wrapping          wrapping   = Wrapping::Repeat;
	WritePixelsFormat format     = WritePixelsFormat::RGBA;
};
