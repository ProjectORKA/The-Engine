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
	[[nodiscard]] Bool          isLoaded() const;
	[[nodiscard]] UInt          getWidth() const;
	[[nodiscard]] UInt          getHeight() const;
	[[nodiscard]] Channels      getChannels() const;
	[[nodiscard]] ImageDataType getDataType() const;
	[[nodiscard]] SizeT         getByteSize() const;
	[[nodiscard]] Byte*         getDataPointer() const;

	void load(const Path& path);
	void loadPNG(const Path& path);
	void loadWebP(const Path& path);
	void loadOther(const Path& path, Bool inverted);
	void advancedWebPLoader(const Byte* fileData, ULL fileSize);

	Image();
	Image(Int width, Int height, Channels channels, ImageDataType dataType, const Memory& pixels);
private:
	Memory        pixelMemory;
	Int           width    = 0;
	Int           height   = 0;
	Bool          loaded   = false;
	Channels      channels = Channels::RGBA;
	ImageDataType dataType = ImageDataType::Byte;
};
