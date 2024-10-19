#include "ImageViewerResource.hpp"

#include "Math.hpp"
#include "FileSystem.hpp"
#include "Hash.hpp"

void ImageViewerResource::destroy(Renderer & renderer)
{
	cpuTexture.unload();
	gpuTexture.unload(renderer);
}

Path ImageViewerResource::getPath() const
{
	return path;
}

String ImageViewerResource::getName() const
{
	return name;
}

void ImageViewerResource::loadOntoGpu(Renderer & renderer)
{
	gpuTexture.load(renderer, cpuTexture);
}

void ImageViewerResource::loadIntoRam()
{
	if (!isBeingLoadedIntoRam)
	{
		isBeingLoadedIntoRam = true;
		cpuTexture.load(getPath(), getFileName(getPath()), Filter::Nearest, Filter::Linear, Wrapping::Clamped);
		if (!cpuTexture.isLoaded())
		{
			loadAttemptFailed = true;
		}
		else
		{
			width  = cpuTexture.getWidth();
			height = cpuTexture.getHeight();
		}
		isBeingLoadedIntoRam = false;
	}
}

Bool ImageViewerResource::inRam() const
{
	return cpuTexture.isLoaded();
}

Bool ImageViewerResource::onGpu() const
{
	return gpuTexture.isLoaded();
}

void ImageViewerResource::unloadFromGpu(Renderer & renderer)
{
	gpuTexture.unload(renderer);
}

void ImageViewerResource::unloadFromRam()
{
	cpuTexture.unload();
}

Float ImageViewerResource::getAspectRatio() const
{
	if (width > 0.0f && height > 0.0f) return static_cast<Float>(width) / static_cast<Float>(height);
	return 1.0f;
}

Int ImageViewerResource::getWidth() const
{
	return width;
}

Int ImageViewerResource::getHeight() const
{
	return height;
}

void ImageViewerResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const auto  b      = static_cast<Float>(resourceCount);
	const auto  a      = static_cast<Float>(currentImageIndex);
	const auto  x      = static_cast<Float>(index);
	const Float diffAx = abs(a - x);
	priority           = b / 2.0f - min(abs(a - x - b), min(diffAx, abs(a - x + b)));
}

Float ImageViewerResource::getPriority() const
{
	return priority;
}

Bool ImageViewerResource::hasLoadAttemptFailed() const
{
	return loadAttemptFailed;
}

void ImageViewerResource::use(const Int textureSlot) const
{
	gpuTexture.useTextureInSlot(textureSlot);
}

ImageViewerResource::ImageViewerResource(const Path& path, const Index index)
{
	this->path  = path;
	this->index = index;
	this->name  = getFileName(getPath());
	this->hash  = calculateSha256Hash(path);
}

String ImageViewerResource::getHash() const
{
	return hash;
}
