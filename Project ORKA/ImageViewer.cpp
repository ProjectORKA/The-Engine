#include "ImageViewer.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"

void ImageViewerResource::destroy()
{
	cpuTexture.unload();
	gpuTexture.unload();
}

Path& ImageViewerResource::getPath()
{
	return path;
}

void ImageViewerRenderer::updateZoom()
{
	const Float oldZoom = zoom;
	zoom                = powf(zoomFactor, static_cast<Float>(zoomLevel));
	offset *= zoom / oldZoom;
}

void ImageViewerResource::loadOntoGpu()
{
	gpuTexture.load(cpuTexture);
}

void ImageViewerResource::loadIntoRam()
{
	cpuTexture.load(path, getFileName(path), Filter::Nearest, Filter::Linear, Wrapping::Clamped);
	if(!cpuTexture.isLoaded()) loadAttemptFailed = true;
}

void ImageViewerRenderer::removeImage()
{
	images.erase(images.begin() + currentImageId);
	if(currentImageId >= images.size()) currentImageId--;
}

Bool ImageViewerResource::inRam() const
{
	return cpuTexture.isLoaded();
}

Bool ImageViewerResource::onGpu() const
{
	return gpuTexture.isLoaded();
}

void ImageViewerResource::unloadFromGpu()
{
	gpuTexture.unload();
}

void ImageViewerResource::unloadFromRam()
{
	cpuTexture.unload();
}

void ImageViewerRenderer::showNextImage()
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if(currentImageId >= static_cast<Int>(images.size())) currentImageId = 0;
	}
}

void ImageViewerRenderer::preLoadImages()
{
	// load resources
	calculatePriorities();

	// if there are images
	if(!images.empty())
	{
		// get vector of indices sorted by priority
		Vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

		// make sure were not loading more than we can
		if(sortedIndices.size() > cpuMaxPreLoadCount) sortedIndices.resize(cpuMaxPreLoadCount);

		// load images
		for(UInt order = 0; order < sortedIndices.size(); order++)
		{
			// get the image index by sorted order
			const Index          currentImageIndex = sortedIndices[order];
			ImageViewerResource& img               = images[currentImageIndex];

			// load all images within the pre load range but unload the rest
			if(order <= cpuMaxPreLoadCount)
			{
				// image is in cpu pre load range

				if(!img.inRam())
				{
					img.loadIntoRam();
					if(img.inRam()) gpuLoadedCount++;
				}

				if(order < gpuMaxPreLoadCount)
				{
					// image is in gpu pre load range
					if(!img.onGpu())
					{
						img.loadOntoGpu();
						if(img.onGpu()) gpuLoadedCount++;
					}
				}
				else
				{
					// image not in gpu pre load range, so unload it
					if(img.onGpu())
					{
						img.unloadFromGpu();
						gpuLoadedCount--;
					}
				}
			}
			else
			{
				// image outside load range

				if(img.onGpu())
				{
					img.unloadFromGpu();
					gpuLoadedCount--;
					logDebug(gpuLoadedCount);
				}

				if(img.inRam())
				{
					img.unloadFromRam();
					cpuLoadedCount--;
					logDebug(cpuLoadedCount);
				}
			}
		}
	}

	// if (cpuLoadedCount >= cpuMaxPreLoadCount - 1)
	// {
	//	if (cpuMaxPreLoadCount < images.size())
	//	{
	//		if (availablePhysicalMemory() > 512 * 1024 * 1024)
	//		{
	//			cpuMaxPreLoadCount++;
	//			gpuMaxPreLoadCount = cpuMaxPreLoadCount;
	//			logDebug(cpuMaxPreLoadCount);
	//		}
	//	}
	// }
}

void ImageViewerRenderer::showPrevImage()
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId--;
		if(currentImageId < 0) currentImageId = images.size() - 1;
	}
}

Int ImageViewerResource::getWidth() const
{
	return cpuTexture.getWidth();
}

Int ImageViewerResource::getHeight() const
{
	return cpuTexture.getHeight();
}

void ImageViewerRenderer::loadGpuResource()
{
	if(!images.empty())
	{
		// if max loaded image count is reached
		if(gpuLoadedCount >= gpuMaxPreLoadCount) unloadGpuImageWithLowestPriority();

		// if there are images available to load into ram and we still have space
		if(gpuLoadedCount <= min(gpuMaxPreLoadCount, images.size())) loadGpuImageWithHighestPriority();
	}
}

void ImageViewerRenderer::loadCpuResource()
{
	if(!images.empty())
	{
		// if max loaded image count is reached
		if(cpuLoadedCount >= cpuMaxPreLoadCount) unloadCpuImageWithLowestPriority();

		// if there are images available to load into ram and we still have space
		if(cpuLoadedCount <= min(cpuMaxPreLoadCount, images.size())) loadCpuImageWithHighestPriority();
	}
}

Float ImageViewerResource::getPriority() const
{
	return priority;
}

void ImageViewerRenderer::calculatePriorities()
{
	for(auto& image : images) image.calculateRating(currentImageId, images.size());
}

void ImageViewerRenderer::update(Window& window)
{
	// load new images if dropped
	if(!window.droppedFilePaths.empty())
	{
		// image path to load folder from
		Path imagePath = "";

		// check for new paths
		for(auto p : window.droppedFilePaths)
		{
			// get last path that is actually an image
			if(isImageFile(p)) imagePath = p;
		}

		// clear queue
		window.droppedFilePaths.clear();

		// only if there is a valid image should we reload resources
		if(imagePath != "")
		{
			// delete currently loaded resource
			for(auto i : images) i.destroy();
			images.clear();

			// get folder
			Path folderPath = getDirectory(imagePath);

			// get all images in folder
			Vector<Path> paths = getFilesInDirectory(folderPath, imageExtensions);

			// set current index to the originally loaded image
			for(Index i = 0; i < paths.size(); i++)
			{
				if(paths[i] == imagePath)
				{
					currentImageId = i;
					break;
				}
			}

			// set images to new paths
			for(Index i = 0; i < paths.size(); i++)
			{
				ImageViewerResource image(paths[i], i);
				images.push_back(image);
			}
		}
	}

	// input
	if(window.pressed(mouseDown)) offset += window.mouseDelta;

	if(window.pressed(nextImageHolding) || window.pressed(nextImageHoldingMouse)) if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showNextImage();

	if(window.pressed(previousImageHolding) || window.pressed(previousImageHoldingMouse)) if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showPrevImage();
}

void ImageViewerRenderer::destroy(Window& window)
{
	// resourceLoaderThreads.waitStop();
}

void ImageViewer::run(const Int argc, Char* argv[])
{
	// file management
	Path         currentPath = getCurrentPath();
	Vector<Path> filePaths;
	// process parameters
	for(Int i = 0; i < argc; i++)
	{
		if(Path path = argv[i]; isExecutableFile(path))
		{
			executablePath = getDirectory(path);
			setCurrentPath(executablePath);
		}
		else
		{
			if(isImageFile(path)) filePaths.push_back(path);
			else logWarning("Can't process input: " + path.string());
		}
	}
	ui.create();
	logDebug(executablePath);
	resourceManager.create();
	auto& windowFilePaths = ui.window("ORKA Image Viewer", Area(1920, 1080), true, true, WindowState::Maximized, renderer, resourceManager).droppedFilePaths;
	for(auto& path : filePaths) if(isImageFile(path)) windowFilePaths.push_back(path);

	ui.run();
}

Bool ImageViewerResource::hasLoadAttemptFailed() const
{
	return loadAttemptFailed;
}

void ImageViewerResource::use(const Int textureSlot) const
{
	gpuTexture.useTextureInSlot(textureSlot);
}

void ImageViewerRenderer::loadGpuImageWithHighestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfGpuImageWithHighestPriority();

		images[imageId].loadOntoGpu();
		if(images[imageId].onGpu()) gpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::loadCpuImageWithHighestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfCpuImageWithHighestPriority();
		images[imageId].loadIntoRam();
		if(images[imageId].inRam()) cpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::unloadGpuImageWithLowestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfGpuImageWithLowestPriority();
		images[imageId].unloadFromGpu();
		if(!images[imageId].onGpu()) gpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void ImageViewerRenderer::unloadCpuImageWithLowestPriority()
{
	if(!images.empty())
	{
		const Index imageId = indexOfCpuImageWithLowestPriority();
		images[imageId].unloadFromRam();
		if(!images[imageId].inRam()) cpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void ImageViewerRenderer::connect(GameSimulation& simulation) {}

Index ImageViewerRenderer::indexOfGpuImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority; // [TODO] fix this
	Index lowestPriorityIndex = 0;
	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].onGpu())
		{
			lowestPriority      = images[i].getPriority();
			lowestPriorityIndex = i;
		}
	}
	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCpuImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority      = 0; // [TODO] fix this
	Index lowestPriorityIndex = 0;

	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].inRam() && !images[i].onGpu())
		{
			lowestPriority      = images[i].getPriority();
			lowestPriorityIndex = i;
		}
	}

	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCpuImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;

	// find the first image that is not yet loaded
	for(Int i = 0; i < images.size(); i++)
	{
		if(!images[i].inRam())
		{
			highestPriority      = images[i].getPriority();
			highestPriorityIndex = i;
			break;
		}
	}

	// get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(!images[i].inRam())
		{
			if(images[i].getPriority() > highestPriority)
			{
				highestPriority      = images[i].getPriority();
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Index ImageViewerRenderer::indexOfGpuImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority      = 0;
	Index highestPriorityIndex = 0;
	// find the first image that is not yet loaded
	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].inRam() && !images[i].onGpu())
		{
			highestPriority      = images[i].getPriority();
			highestPriorityIndex = i;
			break;
		}
	}

	// get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(images[i].inRam() && !images[i].onGpu())
		{
			if(images[i].getPriority() > highestPriority)
			{
				highestPriority      = images[i].getPriority();
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Vector<Index> ImageViewerRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for(Index i = 0; i < images.size(); i++) indices.push_back(i);
	std::sort(indices.begin(), indices.end(), [&](const Index a, const Index b)
	{
		return images[a].getPriority() > images[b].getPriority();
	});
	return indices;
}

void ImageViewerRenderer::inputEvent(Window& window, const InputEvent input)
{
	lastButtonInput = now();

	if(input == zoomIn)
	{
		zoomLevel++;
		updateZoom();
	}

	if(input == zoomOut)
	{
		zoomLevel--;
		updateZoom();
	}

	if(input == nextImage || input == nextImageMouse) showNextImage();

	if(input == previousImage || input == previousImageMouse) showPrevImage();

	if(input == deleteImage)
	{
		if(!images.empty())
		{
			deleteFile(images[currentImageId].getPath());
			images[currentImageId].destroy();
			images.erase(images.begin() + currentImageId);
			if(currentImageId >= images.size()) currentImageId--;
		}
	}

	if(input == resetView)
	{
		offset    = Vec2(0);
		zoomLevel = 0;
		updateZoom();
	}
}

ImageViewerResource::ImageViewerResource(const Path& path, const Index index)
{
	this->path  = path;
	this->index = index;
}

void ImageViewerRenderer::create(ResourceManager& resourceManager, Window& window)
{
	// resourceLoaderThreads.start();
}

void ImageViewerResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const Float b      = static_cast<Float>(resourceCount);
	const Float a      = static_cast<Float>(currentImageIndex);
	const Float x      = static_cast<Float>(index);
	const Float diffAx = abs(a - x);
	priority           = b / 2.0f - min(abs(a - x - b), min(diffAx, abs(a - x + b)));
}

void ImageViewerRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.uniforms().reset();

	renderer.clearBackground(Color(0.08, 0.08, 0.08, 1));

	renderer.setAlphaBlending(true, BlendFunction::One, BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);

	renderer.normalizedScreenSpace();

	Float imgX = 0;
	Float imgY = 0;

	if(!images.empty() && images[currentImageId].onGpu())
	{
		images[currentImageId].use(0);
		imgX = images[currentImageId].getWidth();
		imgY = images[currentImageId].getHeight();
	}

	const auto winX = static_cast<Float>(renderer.getWindowWidth());
	const auto winY = static_cast<Float>(renderer.getWindowHeight());

	const Float size = min(winX / imgX, winY / imgY);

	auto finalMatrix = Matrix(1);
	finalMatrix      = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix      = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix      = scale(finalMatrix, Vec3(zoom));

	approach(actualMatrix, finalMatrix, renderer.deltaTime() * 30);
	actualMatrix = finalMatrix;

	renderer.uniforms().setMMatrix(actualMatrix);

	renderer.useShader(resourceManager, "imageViewer");
	renderer.renderMesh(resourceManager, "centeredPlane");

	preLoadImages();

	for(Index i = 0; i < images.size(); i++) if(images[i].hasLoadAttemptFailed()) images.erase(images.begin() + i);
}

void ImageViewerRenderer::renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) { }
