#pragma once

#include "Basics.hpp"
#include "Memory.hpp"

enum class ImageDataType : Byte
{
	Byte  = 1,
	Float = 2,
	UInt  = 3,
	Int   = 4,
};

enum class Channels : Byte
{
	Red  = 1,
	RG   = 2,
	RGB  = 3,
	RGBA = 4,
};

struct Image
{
	[[nodiscard]] Bool     isLoaded() const;
	[[nodiscard]] UInt     getWidth() const;
	[[nodiscard]] UInt     getHeight() const;
	[[nodiscard]] Channels getChannels() const;

	[[nodiscard]] SizeT         getByteSize() const;
	[[nodiscard]] ImageDataType getDataType() const;
	[[nodiscard]] Byte*         getDataPointer() const;
	[[nodiscard]] Int           getChannelCount() const;

	Image();
	void load(const Path& path);
	void flipVertically() const;
	Image(Int width, Int height, Channels channels, ImageDataType dataType, const Memory& pixels);

private:
	Memory        pixelMemory;
	Int           width    = 0;
	Int           height   = 0;
	Bool          loaded   = false;
	Channels      channels = Channels::RGBA;
	ImageDataType dataType = ImageDataType::Byte;

	void loadBMP(const Path& path);
	void loadEXR(const Path& path);
	void loadHDR(const Path& path);
	void loadPNG(const Path& path);
	void loadJPEG(const Path& path);
	void loadJFIF(const Path& path);
	void loadTIFF(const Path& path);
	void loadWebP(const Path& path);
	void loadGIF(const Path& path, Int frameId);
	void loadOther(const Path& path, Bool inverted);
	void create(Int width, Int height, Channels channels, Int frames, ImageDataType dataType, const Memory& pixels);
};
