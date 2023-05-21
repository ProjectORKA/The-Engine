#include "ImageViewer.hpp"
#include "FileTypes.hpp"
#include "System.hpp"

ImageViewer::ImageViewer(const Int argc, Char* argv[])
{
	//file management
	Path currentPath = getDirectory(std::filesystem::current_path());
	Vector<Path> filePaths;

	// process parameters
	for(Index i = 0; i < argc; i++)
	{
		Path path = argv[i];
		if(isExecutableFile(path))
		{
			executablePath = getDirectory(path);
			current_path(executablePath);
		}
		else
		{
			if(isImageFile(path))
			{
				filePaths.push_back(path);
			}
			else
			{
				logWarning(String("Can't process input: ").append(path.string()));
			}
		}
	}

	ui.create();

	logDebug(executablePath);

	resourceManager.create();

	Window& w = ui.window("ORKA Image Viewer", Area(1920, 1080), true, WindowState::Maximized, renderer, resourceManager);

	for(auto& path : filePaths)
	{
		if(isImageFile(path))
		{
			w.droppedFilePaths.push_back(path);
		}
	}

	ui.run();
}

void ImageViewerResource::destroy()
{
	cpuTexture.unload();
	gpuTexture.unload();
}

void ImageViewerResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const Float b = resourceCount;
	const Float a = currentImageIndex;
	const Float x = index;
	const Float diff_a_x = abs(a - x);
	priority = b / 2.0 - min(abs(a - x - b), min(diff_a_x, abs(a - x + b)));
}

void ImageViewerRenderer::updateZoom()
{
	const Float oldZoom = zoom;
	zoom = pow(zoomfactor, zoomLevel);
	offset *= zoom / oldZoom;
}

void ImageViewerRenderer::showNextImage()
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if(currentImageId >= images.size()) currentImageId = 0;
	}
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

void ImageViewerRenderer::loadGPUResource()
{
	if(! images.empty())
	{
		//if max loaded image count is reached
		if(gpuLoadedCount >= gpuMaxPreloadCount)
		{
			unloadGPUImageWithLowestPriority();
		}

		//if there are images available to load into ram and we still have space
		if(gpuLoadedCount <= min(gpuMaxPreloadCount, images.size()))
		{
			loadGPUImageWithHighestPriority();
		}
	}
}

void ImageViewerRenderer::loadCPUResource()
{
	if(! images.empty())
	{
		//if max loaded image count is reached
		if(cpuLoadedCount >= cpuMaxPreloadCount)
		{
			unloadCPUImageWithLowestPriority();
		}

		//if there are images available to load into ram and we still have space
		if(cpuLoadedCount <= min(cpuMaxPreloadCount, images.size()))
		{
			loadCPUImageWithHighestPriority();
		}
	}
}

void ImageViewerRenderer::calculatePriorities()
{
	for(auto& image : images)
	{
		image.calculateRating(currentImageId, images.size());
	}
}

void ImageViewerRenderer::update(Window& window)
{
	//load new images if dropped
	if(! window.droppedFilePaths.empty())
	{
		//image path to load folder from
		Path imagePath = "";

		//check for new paths
		for(auto p : window.droppedFilePaths)
		{
			//get last path that is actually an image
			if(isImageFile(p)) imagePath = p;
		}

		//clear queue
		window.droppedFilePaths.clear();

		//only if there is a valid image should we reload resources
		if(imagePath != "")
		{
			//delete currently loaded resource
			for(auto i : images) i.destroy();
			images.clear();

			//get folder
			Path folderPath = getDirectory(imagePath);

			//get all images in folder
			Vector<Path> paths = getFilesInDirectory(folderPath, imageExtensions);

			//set current index to the originally loaded image
			for(Index i = 0; i < paths.size(); i++)
			{
				if(paths[i] == imagePath)
				{
					currentImageId = i;
					break;
				}
			}

			//set images to new paths
			for(Index i = 0; i < paths.size(); i++)
			{
				ImageViewerResource image;
				image.path = paths[i];
				image.index = i;
				images.push_back(image);
			}
		}
	}

	//input
	if(window.pressed(mouseDown))
	{
		offset += window.mouseDelta;
	}

	if(window.pressed(nextImageHolding) || window.pressed(nextImageHoldingMouse))
	{
		if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showNextImage();
	}

	if(window.pressed(previousImageHolding) || window.pressed(previousImageHoldingMouse))
	{
		if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showPrevImage();
	}
}

void ImageViewerRenderer::loadGPUImageWithHighestPriority()
{
	if(! images.empty())
	{
		const Index imageId = indexOfGPUImageWithHighestPriority();
		images[imageId].gpuTexture.load(images[imageId].cpuTexture);
		if(images[imageId].gpuTexture.loaded) gpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::loadCPUImageWithHighestPriority()
{
	if(! images.empty())
	{
		const Index imageId = indexOfCPUImageWithHighestPriority();
		images[imageId].cpuTexture.load(images[imageId].path, "imageViewerImage", Filter::nearest, Filter::linear, Wrapping::clamped);
		if(images[imageId].cpuTexture.loaded) cpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::unloadGPUImageWithLowestPriority()
{
	if(! images.empty())
	{
		const Index imageId = indexOfGPUImageWithLowestPriority();
		images[imageId].gpuTexture.unload();
		if(! images[imageId].gpuTexture.loaded) gpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void ImageViewerRenderer::unloadCPUImageWithLowestPriority()
{
	if(! images.empty())
	{
		const Index imageId = indexOfCPUImageWithLowestPriority();
		images[imageId].cpuTexture.unload();
		if(! images[imageId].cpuTexture.loaded) cpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void ImageViewerRenderer::preloadImage(const Path& path, GpuTexture& texture)
{
	if(texture.loaded) texture.unload();
	CPUTexture cpuTexture;
	cpuTexture.unload();
	cpuTexture.nearFilter = Filter::nearest;
	cpuTexture.wrapping = Wrapping::clamped;
	cpuTexture.load(path, "imageViewerImage", Filter::nearest, Filter::linear, Wrapping::clamped);
	texture.load(cpuTexture);
};

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

	if(input == nextImage || input == nextImageMouse)
	{
		showNextImage();
	}

	if(input == previousImage || input == previousImageMouse)
	{
		showPrevImage();
	}

	if(input == deleteImage)
	{
		if(! images.empty())
		{
			deleteFile(images[currentImageId].path);
			images[currentImageId].destroy();
			images.erase(images.begin() + currentImageId);
			if(currentImageId >= images.size()) currentImageId--;
		}
	}

	if(input == resetView)
	{
		offset = Vec2(0);
		zoomLevel = 0;
		updateZoom();
	}
}

void ImageViewerRenderer::preloadImages()
{
	//load resources
	calculatePriorities();

	//if there are images
	if(! images.empty())
	{
		//get vector of indices sorted by priority
		Vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

		//make sure were not loading more than we can
		if(sortedIndices.size() > cpuMaxPreloadCount) sortedIndices.resize(cpuMaxPreloadCount);

		//load images
		for(UInt order = 0; order < sortedIndices.size(); order++)
		{
			//get the image index by sorted order
			const Index currentImageIndex = sortedIndices[order];

			//load all images within the preload range but unload the rest
			if(order <= cpuMaxPreloadCount)
			{
				//load cpu image
				if(! images[currentImageIndex].cpuTexture.loaded)
				{
					images[currentImageIndex].cpuTexture.load(images[currentImageIndex].path, "imageViewerImage", Filter::nearest, Filter::linear, Wrapping::clamped);
					cpuLoadedCount++;
					logDebug(cpuLoadedCount);
				}

				if(order < gpuMaxPreloadCount)
				{
					//load gpu image
					if(! images[currentImageIndex].gpuTexture.loaded)
					{
						images[currentImageIndex].gpuTexture.load(images[currentImageIndex].cpuTexture);
						gpuLoadedCount++;
						logDebug(gpuLoadedCount);
					}
				}
				else
				{
					//unload gpu image
					if(images[currentImageIndex].gpuTexture.loaded)
					{
						images[currentImageIndex].gpuTexture.unload();
						gpuLoadedCount--;
						logDebug(gpuLoadedCount);
					}
				}
			}
			else
			{
				//unload cpu image
				if(images[currentImageIndex].cpuTexture.loaded)
				{
					images[currentImageIndex].cpuTexture.unload();
					cpuLoadedCount--;
					logDebug(cpuLoadedCount);
				}

				//unload gpu image just to be safe
				if(images[currentImageIndex].gpuTexture.loaded)
				{
					images[currentImageIndex].gpuTexture.unload();
					gpuLoadedCount--;
					logDebug(gpuLoadedCount);
				}
			}
		}
	}

	if(cpuLoadedCount >= cpuMaxPreloadCount - 1)
	{
		if(cpuMaxPreloadCount < images.size())
		{
			if(availablePhysicalMemory() > 512 * 1024 * 1024)
			{
				cpuMaxPreloadCount++;
				gpuMaxPreloadCount = cpuMaxPreloadCount;
				logDebug(cpuMaxPreloadCount);
			}
		}
	}
}

void ImageViewerRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	renderer.uniforms().reset();

	renderer.draw("main");

	renderer.clearColor(Color(0.08, 0.08, 0.08, 1));

	renderer.setAlphaBlending(true, BlendFunction::One, BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);

	renderer.normalizedScreenSpace();

	Float imgX = 0;
	Float imgY = 0;

	if(! images.empty())
	{
		images[currentImageId].gpuTexture.use(0);
		imgX = images[currentImageId].cpuTexture.width;
		imgY = images[currentImageId].cpuTexture.height;
	}

	const Float winX = renderer.renderRegion.region.size.x;
	const Float winY = renderer.renderRegion.region.size.y;

	const Float size = min(winX / imgX, winY / imgY);

	auto finalMatrix = Matrix(1);
	finalMatrix = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix = scale(finalMatrix, Vec3(zoom));

	//approach(actualMatrix, finalMatrix, renderer.deltaTime() * 20);
	actualMatrix = finalMatrix;

	renderer.uniforms().mMatrix() = actualMatrix;

	renderer.useShader(resourceManager, "imageViewer");
	renderer.renderMesh(resourceManager, "centeredPlane");

	preloadImages();
}

Index ImageViewerRenderer::indexOfGPUImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority = 0;
	Index lowestPriorityIndex = 0;
	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].gpuTexture.loaded)
		{
			lowestPriority = images[i].priority;
			lowestPriorityIndex = i;
		}
	}
	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCPUImageWithLowestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float lowestPriority = 0;
	Index lowestPriorityIndex = 0;

	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].cpuTexture.loaded && images[i].gpuTexture.loaded != true)
		{
			lowestPriority = images[i].priority;
			lowestPriorityIndex = i;
		}
	}

	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCPUImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority = 0;
	Index highestPriorityIndex = 0;

	//find the first image that is not yet loaded
	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].cpuTexture.loaded != true)
		{
			highestPriority = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(images[i].cpuTexture.loaded != true)
		{
			if(images[i].priority > highestPriority)
			{
				highestPriority = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Index ImageViewerRenderer::indexOfGPUImageWithHighestPriority() const
{
	if(images.empty()) logError("No images to get index from!");

	Float highestPriority = 0;
	Index highestPriorityIndex = 0;
	//find the first image that is not yet loaded
	for(Int i = 0; i < images.size(); i++)
	{
		if(images[i].cpuTexture.loaded && ! images[i].gpuTexture.loaded)
		{
			highestPriority = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for(Index i = highestPriorityIndex; i < images.size(); i++)
	{
		if(images[i].cpuTexture.loaded && ! images[i].gpuTexture.loaded)
		{
			if(images[i].priority > highestPriority)
			{
				highestPriority = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Vector<Index> ImageViewerRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for(Index i = 0; i < images.size(); i++)
	{
		indices.push_back(i);
	}
	std::sort(indices.begin(), indices.end(), [&](const Index a, const Index b)
	{
		return images[a].priority > images[b].priority;
	});
	return indices;
}