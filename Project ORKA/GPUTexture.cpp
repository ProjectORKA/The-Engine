#include "GPUTexture.hpp"
#include "AIPlayground.hpp"

void GPUTexture::unload()
{
	if(loaded)
	{
		openglTexture.destroy();
		loaded = false;
	}
}

Bool GPUTexture::isLoaded() const
{
	return loaded;
}

UInt GPUTexture::getOpenGLID() const
{
	return openglTexture.getID();
}

Float GPUTexture::getAspectRatio() const
{
	if (width > 0.0f && height > 0.0f) return static_cast<Float>(width) / static_cast<Float>(height);
	return 1.0f;
}

void GPUTexture::resize(const Area size)
{
	openglTexture.destroy();
	width  = size.x;
	height = size.y;
	openglTexture.create(toString(name));
	setData(dataType, format, width, height, nullptr);
}

void GPUTexture::generateMipMaps() const
{
	if(farFilter == Filter::LinearMm || farFilter == Filter::NearestMm) openglTexture.generateMipMap();
}

WritePixelsFormat GPUTexture::getFormat() const
{
	return format;
}

void GPUTexture::load(const CpuTexture& cpuTexture)
{
	if(cpuTexture.isLoaded())
	{
		if(loaded) unload();

		name       = cpuTexture.getName();
		wrapping   = cpuTexture.getWrapping();
		nearFilter = cpuTexture.getNearFilter();
		farFilter  = cpuTexture.getFarFilter();
		format     = cpuTexture.getFormat();
		dataType   = cpuTexture.getDataType();
		width      = cpuTexture.getWidth();
		height     = cpuTexture.getHeight();

		// create texture
		openglTexture.create(toString(cpuTexture.getName()));

		setData(cpuTexture.getDataType(), cpuTexture.getFormat(), cpuTexture.getWidth(), cpuTexture.getHeight(), cpuTexture.getPixels());

		openglTexture.setWrapping(wrapping);

		if(wrapping == Wrapping::Border) openglTexture.setBorderColor(Color(0.0f, 0.0f, 0.0f, 1.0f));

		generateMipMaps(nearFilter, farFilter);

		loaded = true;
	}
	else
	{
		logWarning("CPUTexture not loaded!");
	}
}

void GPUTexture::useTextureInSlot(const UInt textureSlot) const
{
	if(loaded)
	{
		openglTexture.useTextureInSlot(textureSlot);
	}
	else
	{
		openglTexture.emptyTextureFromSlot(textureSlot);
	}
}

void GPUTexture::load(const Name& name)
{
	CpuTexture t;
	t.load(name, Filter::Linear, Filter::LinearMm, Wrapping::Repeat);
	load(t);
}

void GPUTexture::setFilters(const Filter nearFilterValue, const Filter farFilterValue)
{
	nearFilter = nearFilterValue;
	farFilter  = farFilterValue;
	openglTexture.setFilters(nearFilter, farFilter);
}

void GPUTexture::generateMipMaps(const Filter nearFilterValue, const Filter farFilterValue)
{
	setFilters(nearFilterValue, farFilterValue);
	generateMipMaps();
}

void GPUTexture::load(const IVec2 size, const WritePixelsFormat format, const DataType type, const Wrapping wrapping)
{
	const CpuTexture cpuTexture(size.x, size.y, "generated", Filter::Linear, Filter::Linear, type, wrapping, format);
	load(cpuTexture);
}

void GPUTexture::setData(const DataType dataType, const WritePixelsFormat format, const Int width, const Int height, const Byte* data)
{
	// [TODO] add support for loading depth textures, right now they are just empty

	auto internalFormat = SizedInternalFormat::RGBA8;
	this->dataType      = dataType;
	this->format        = format;
	this->width         = width;
	this->height        = height;

	switch(format)
	{
		case WritePixelsFormat::Red:
		case WritePixelsFormat::RedInteger:
			switch(dataType)
			{
				case DataType::Byte:
					internalFormat = SizedInternalFormat::R8;
					break;
				case DataType::Float:
					internalFormat = SizedInternalFormat::R32F;
					break;
				case DataType::UInt:
					internalFormat = SizedInternalFormat::R32UI;
					break;
				default:
					logError("Format not supported!");
					break;
			}
			break;
		case WritePixelsFormat::RG:
		case WritePixelsFormat::RGInteger:
			switch(dataType)
			{
				case DataType::Byte:
					internalFormat = SizedInternalFormat::RG8;
					break;
				case DataType::Float:
					internalFormat = SizedInternalFormat::RG32F;
					break;
				case DataType::UInt:
					internalFormat = SizedInternalFormat::RG32UI;
					break;
				default:
					logError("Format not supported!");
					break;
			}
			break;
		case WritePixelsFormat::RGB:
		case WritePixelsFormat::RGBInteger:
			switch(dataType)
			{
				case DataType::Byte:
					internalFormat = SizedInternalFormat::RGB8;
					break;
				case DataType::Float:
					internalFormat = SizedInternalFormat::RGB32F;
					break;
				case DataType::UInt:
					internalFormat = SizedInternalFormat::RGB32UI;
					break;
				default:
					logError("Format not supported!");
					break;
			}
			break;
		case WritePixelsFormat::RGBA:
		case WritePixelsFormat::RGBAInteger:
			switch(dataType)
			{
				case DataType::Byte:
					internalFormat = SizedInternalFormat::RGBA8;
					break;
				case DataType::Float:
					internalFormat = SizedInternalFormat::RGBA32F;
					break;
				case DataType::UInt:
					internalFormat = SizedInternalFormat::RGBA32UI;
					break;
				default:
					logError("Format not supported!");
					break;
			}
			break;
		case WritePixelsFormat::Depth:
			openglTexture.setDataToDepth(width, height, nullptr);
			return;
		case WritePixelsFormat::DepthStencil:
		case WritePixelsFormat::LuminanceAlpha:
		case WritePixelsFormat::Luminance:
		case WritePixelsFormat::Alpha:
			logError("Format not supported!");
			break;
	}

	openglTexture.setData(internalFormat, width, height, format, dataType, data);
}
