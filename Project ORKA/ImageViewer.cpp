#include "ImageViewer.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "JobSystem.hpp"
#include "Platform.hpp"

void ImageViewerResource::destroy()
{
	cpuTexture.unload();
	gpuTexture.unload();
}

Path& ImageViewerResource::getPath()
{
	return path;
}

String ImageViewerResource::getName()
{
	return getFileName(getPath());
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
	if(!isBeingLoadedIntoRam)
	{
		isBeingLoadedIntoRam = true;
		if(debugImageLoadingIsEnabled) logDebug("Loading image (" + getName() + ") into RAM!");
		cpuTexture.load(path, getFileName(path), Filter::Nearest, Filter::Linear, Wrapping::Clamped);
		if(!cpuTexture.isLoaded()) loadAttemptFailed = true;
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

void ImageViewerResource::unloadFromGpu()
{
	gpuTexture.unload();
}

void ImageViewerResource::unloadFromRam()
{
	cpuTexture.unload();
}

Int ImageViewerResource::getWidth() const
{
	return cpuTexture.getWidth();
}

void ImageViewerRenderer::showNextImage(const Window& window)
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if(currentImageId >= static_cast<Int>(images.size())) currentImageId = 0;
		window.setTitle(windowTitleSubstring + images[currentImageId].getName());

		updateLoadedImages();
	}
}

void ImageViewerRenderer::updateLoadedImages()
{
	calculatePriorities();
	const Vector<Index> sortedImageIds         = indicesOfImagesSortedByPriority();
	Int                 firstUnloadedFromFront = 0;
	Int                 firstLoadedFromBack    = sortedImageIds.size() - 1;
	while(images[sortedImageIds[firstUnloadedFromFront]].inRam() && firstUnloadedFromFront < sortedImageIds.size() - 1) firstUnloadedFromFront++;
	while(!images[sortedImageIds[firstLoadedFromBack]].inRam() && firstLoadedFromBack >= 0) firstLoadedFromBack--;

	if(firstUnloadedFromFront != firstLoadedFromBack)
	{
		images[sortedImageIds[firstUnloadedFromFront]].loadIntoRam();
		images[sortedImageIds[firstLoadedFromBack]].unloadFromRam();
		logDebug("Swapped" + toString(firstUnloadedFromFront) + " with " + toString(firstLoadedFromBack));
	}
}

void ImageViewerRenderer::showPrevImage(const Window& window)
{
	const auto currentTime = now();
	if(lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime)
	{
		lastImageRefresh = currentTime;
		currentImageId--;
		if(currentImageId < 0) currentImageId = images.size() - 1;
		window.setTitle(windowTitleSubstring + images[currentImageId].getName());

		updateLoadedImages();
	}
}

void ImageViewerRenderer::create(Window& window)
{
	availableMemoryUponStartup = availablePhysicalMemoryInBytes();
}

Int ImageViewerResource::getHeight() const
{
	return cpuTexture.getHeight();
}

void ImageViewerRenderer::destroy(Window& window) {}

Float ImageViewerResource::getPriority() const
{
	return priority;
}

void loadNextImage(ImageViewerRenderer& viewer)
{
	if(!viewer.images.empty())
	{
		SharedLockGuard sharedLockGuard(viewer.imagesMutex);
		for(ImageViewerResource& i : viewer.images)
		{
			if(!i.inRam())
			{
				i.loadIntoRam();

				return;
			}
		}
	}
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
			LockGuard lockGuard(imagesMutex);

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

			// set title to current image
			window.setTitle(windowTitleSubstring + images[currentImageId].getName());

			// determine the importance of images such that we load the ones we need first
			calculatePriorities();

			// create the load order
			Vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

			for(auto n : sortedIndices) jobSystem.enqueue(loadImage, std::ref(*this), n);
		}
	}

	// input
	if(window.pressed(mouseDown)) offset += window.mouseDelta;

	if(window.pressed(nextImageHolding) || window.pressed(nextImageHoldingMouse)) if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showNextImage(window);

	if(window.pressed(previousImageHolding) || window.pressed(previousImageHoldingMouse)) if(now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showPrevImage(window);
}

void ImageViewer::run(const Int argc, Char* argv[])
{
	ui.create();

	// scan launch parameters for input
	for(Int i = 0; i < argc; i++)
	{
		Path path = argv[i];

		if(isExecutableFile(path)) continue;

		if(isImageFile(path)) filePaths.push_back(path);
		else logWarning("Can't process input: " + path.string());
	}

	Window& window = ui.window("ORKA Image Viewer", Area(1920, 1080), true, true, WindowState::Windowed, renderer);

	for(auto& path : filePaths) window.droppedFilePaths.push_back(path);

	ui.run();
}

Bool ImageViewerResource::hasLoadAttemptFailed() const
{
	return loadAttemptFailed;
}

void ImageViewerRenderer::connect(GameSimulation& simulation) {}

void ImageViewerResource::use(const Int textureSlot) const
{
	gpuTexture.useTextureInSlot(textureSlot);
}

void loadImage(ImageViewerRenderer& viewer, const Index imageID)
{
	if(availablePhysicalMemoryInBytes() > viewer.availableMemoryUponStartup / 2) viewer.images[imageID].loadIntoRam();
	else logWarning("Image could not be loaded! RAM is full!");
}

Vector<Index> ImageViewerRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for(Index i = 0; i < images.size(); i++) indices.push_back(i);
	std::ranges::sort(indices, [&](const Index a, const Index b)
	{
		return images[a].getPriority() > images[b].getPriority();
	});
	return indices;
}

void ImageViewerRenderer::renderInteractive(Window& window, TiledRectangle area) {}

void ImageViewerRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	// images need to be loaded onto gpu in this thread, so lets just do it before we render
	for(Int i = 0; i < images.size(); i++) if(images[i].inRam() && !images[i].onGpu()) images[i].loadOntoGpu();

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

	if(smoothCameraTransition) approach(actualMatrix, finalMatrix, renderer.deltaTime() * smoothCameraSpeed);
	else actualMatrix = finalMatrix;

	renderer.uniforms().setMMatrix(actualMatrix);

	renderer.useShader("imageViewer");
	renderer.renderMesh("centeredPlane");

	if(images.size() > 10) renderAllImages(window, area);
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

	if(input == nextImage || input == nextImageMouse) showNextImage(window);

	if(input == previousImage || input == previousImageMouse) showPrevImage(window);

	//if(input == deleteImage)
	//{
	//	if(!images.empty())
	//	{
	//		deleteFile(images[currentImageId].getPath());
	//		images[currentImageId].destroy();
	//		//images.erase(images.begin() + currentImageId);
	//		if(currentImageId >= images.size()) currentImageId--;
	//	}
	//}

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

void ImageViewerRenderer::renderAllImages(Window& window, const TiledRectangle area)
{
	window.renderer.screenSpace();

	Float xOffset = 0;
	Float yOffset = 0;

	for(Int i = 0; i < images.size(); i++)
	{
		ImageViewerResource& image = images[i];

		if(image.onGpu()){
			if(currentImageId == i) yOffset = desiredHeight / 2;
			else yOffset = 0;
			Float       width  = static_cast<Float>(image.getWidth());
			const Float height = static_cast<Float>(image.getHeight());
			width              = width / height * desiredHeight;
			image.use(0);
			window.renderer.rectangle(Vec2(xOffset, yOffset), Vec2(width, desiredHeight), false, false);
			xOffset += width;
		}
	}
	if(xOffset > 0.0) desiredHeight *= static_cast<Float>(area.size.x) / xOffset;
	desiredHeight = min(desiredHeight, static_cast<Float>(area.size.y) * 0.05f);
}

void ImageViewerResource::calculateRating(const Index currentImageIndex, const UInt resourceCount)
{
	const auto  b      = static_cast<Float>(resourceCount);
	const auto  a      = static_cast<Float>(currentImageIndex);
	const auto  x      = static_cast<Float>(index);
	const Float diffAx = abs(a - x);
	priority           = b / 2.0f - min(abs(a - x - b), min(diffAx, abs(a - x + b)));
}

//void ImageViewerRenderer::removeImage()
//{
//	//images.erase(images.begin() + currentImageId);
//	images.erase(images.begin() + currentImageId);
//	if(currentImageId >= images.size()) currentImageId--;
//}
