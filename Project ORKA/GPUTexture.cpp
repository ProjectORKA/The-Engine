#include "GPUTexture.hpp"
#include "AIPlayground.hpp"
#include "Renderer.hpp"
#include "Settings.hpp"

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

void GPUTexture::unload(Renderer& renderer)
{
	if (loaded)
	{
		if (debugGpuTextureIsEnabled) logDebug("Unloading GPUTexture: " + toString(name));
		openglTexture.destroy(renderer.openGlContext);
		loaded = false;
	}
}

WritePixelsFormat GPUTexture::getFormat() const
{
	return format;
}

void GPUTexture::generateMipMaps(Renderer& renderer) const
{
	if (farFilter == Filter::LinearMm || farFilter == Filter::NearestMm) openglTexture.generateMipMap(renderer.openGlContext);
}

void GPUTexture::load(Renderer& renderer, const Name& name)
{
	CpuTexture t;
	t.load(name, Filter::Linear, Filter::LinearMm, Wrapping::Repeat);
	load(renderer, t);
}

void GPUTexture::resize(Renderer& renderer, const IVec2 size)
{
	openglTexture.destroy(renderer.openGlContext);
	width  = size.x;
	height = size.y;
	openglTexture.create(renderer.openGlContext, toString(name));
	setData(renderer, dataType, format, width, height, nullptr);
}

void GPUTexture::load(Renderer& renderer, const CpuTexture& cpuTexture)
{
	if (cpuTexture.isLoaded())
	{
		if (loaded) unload(renderer);

		if (debugGpuTextureIsEnabled) logDebug("Loading GPUTexture: " + toString(cpuTexture.getName()));

		name       = cpuTexture.getName();
		wrapping   = cpuTexture.getWrapping();
		nearFilter = cpuTexture.getNearFilter();
		farFilter  = cpuTexture.getFarFilter();
		format     = cpuTexture.getFormat();
		dataType   = cpuTexture.getDataType();
		width      = cpuTexture.getWidth();
		height     = cpuTexture.getHeight();

		// create texture
		openglTexture.create(renderer.openGlContext, toString(cpuTexture.getName()));

		setData(renderer, cpuTexture.getDataType(), cpuTexture.getFormat(), cpuTexture.getWidth(), cpuTexture.getHeight(), cpuTexture.getPixels());

		openglTexture.setWrapping(renderer.openGlContext, wrapping);

		if (wrapping == Wrapping::Border) openglTexture.setBorderColor(renderer.openGlContext, Color(0.0f, 0.0f, 0.0f, 1.0f));

		generateMipMaps(renderer, nearFilter, farFilter);

		loaded = true;
	}
	else logWarning("CPUTexture not loaded!");
}

void GPUTexture::useTextureInSlot(Renderer& renderer, const UInt textureSlot) const
{
	if (loaded) openglTexture.useTextureInSlot(renderer.openGlContext, textureSlot);
	else openglTexture.emptyTextureFromSlot(renderer.openGlContext, textureSlot);
}

void GPUTexture::setFilters(Renderer& renderer, const Filter nearFilterValue, const Filter farFilterValue)
{
	nearFilter = nearFilterValue;
	farFilter  = farFilterValue;
	openglTexture.setFilters(renderer.openGlContext, nearFilter, farFilter);
}

void GPUTexture::generateMipMaps(Renderer& renderer, const Filter nearFilterValue, const Filter farFilterValue)
{
	setFilters(renderer, nearFilterValue, farFilterValue);
	generateMipMaps(renderer);
}

void GPUTexture::load(Renderer& renderer, const IVec2 size, const WritePixelsFormat format, const DataType type, const Wrapping wrapping)
{
	const CpuTexture cpuTexture(size.x, size.y, "generated", Filter::Linear, Filter::Linear, type, wrapping, format);
	load(renderer, cpuTexture);
}

void GPUTexture::setData(Renderer& renderer, const DataType dataType, const WritePixelsFormat format, const Int width, const Int height, const Byte* data)
{
	// [TODO] add support for loading depth textures, right now they are just empty

	auto internalFormat = SizedInternalFormat::RGBA8;
	this->dataType      = dataType;
	this->format        = format;
	this->width         = width;
	this->height        = height;

	switch (format)
	{
	case WritePixelsFormat::Red:
	case WritePixelsFormat::RedInteger: switch (dataType)
		{
		case DataType::Byte: internalFormat = SizedInternalFormat::R8;
			break;
		case DataType::Float: internalFormat = SizedInternalFormat::R32F;
			break;
		case DataType::UInt: internalFormat = SizedInternalFormat::R32UI;
			break;
		default: logError("Format not supported!");
			break;
		}
		break;
	case WritePixelsFormat::RG:
	case WritePixelsFormat::RGInteger: switch (dataType)
		{
		case DataType::Byte: internalFormat = SizedInternalFormat::RG8;
			break;
		case DataType::Float: internalFormat = SizedInternalFormat::RG32F;
			break;
		case DataType::UInt: internalFormat = SizedInternalFormat::RG32UI;
			break;
		default: logError("Format not supported!");
			break;
		}
		break;
	case WritePixelsFormat::RGB:
	case WritePixelsFormat::RGBInteger: switch (dataType)
		{
		case DataType::Byte: internalFormat = SizedInternalFormat::RGB8;
			break;
		case DataType::Float: internalFormat = SizedInternalFormat::RGB32F;
			break;
		case DataType::UInt: internalFormat = SizedInternalFormat::RGB32UI;
			break;
		default: logError("Format not supported!");
			break;
		}
		break;
	case WritePixelsFormat::RGBA:
	case WritePixelsFormat::RGBAInteger: switch (dataType)
		{
		case DataType::Byte: internalFormat = SizedInternalFormat::RGBA8;
			break;
		case DataType::Float: internalFormat = SizedInternalFormat::RGBA32F;
			break;
		case DataType::UInt: internalFormat = SizedInternalFormat::RGBA32UI;
			break;
		default: logError("Format not supported!");
			break;
		}
		break;
	case WritePixelsFormat::Depth: openglTexture.setDataToDepth(renderer.openGlContext, width, height, nullptr);
		return;
	case WritePixelsFormat::DepthStencil:
	case WritePixelsFormat::LuminanceAlpha:
	case WritePixelsFormat::Luminance:
	case WritePixelsFormat::Alpha: logError("Format not supported!");
		break;
	}

	openglTexture.setData(renderer.openGlContext, internalFormat, width, height, format, dataType, data);
}
