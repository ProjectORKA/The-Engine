#include "ImageViewer.hpp"
#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "JobSystem.hpp"
#include "Platform.hpp"

void ImageViewerRenderer::updateZoom()
{
	const Float oldZoom = zoom;
	zoom                = powf(zoomFactor, static_cast<Float>(zoomLevel));
	offset *= zoom / oldZoom;
}

void ImageViewerRenderer::removeImage()
{
	imagesMutex.lock();
	images[currentImageId].destroy();
	images.erase(images.begin() + currentImageId);
	if (currentImageId >= static_cast<Int>(images.size())) currentImageId--;
	imagesMutex.unlock();
}

void ImageViewerRenderer::destroy(Window& window) {}

void ImageViewerRenderer::updateLoadedImages()
{
	if (!images.empty())
	{
		imagesMutex.lock();
		calculatePriorities();
		const Vector<Index> sortedImageIds         = indicesOfImagesSortedByPriority();
		Int                 firstUnloadedFromFront = 0;
		Int                 firstLoadedFromBack    = static_cast<Int>(sortedImageIds.size()) - 1;

		while (firstUnloadedFromFront < static_cast<Int>(sortedImageIds.size()) - 1 && images[sortedImageIds[firstUnloadedFromFront]].inRam()) firstUnloadedFromFront++;

		while (firstLoadedFromBack > 0 && !images[sortedImageIds[firstLoadedFromBack]].inRam()) firstLoadedFromBack--;

		if (firstUnloadedFromFront != firstLoadedFromBack)
		{
			loadImage(*this, sortedImageIds[firstUnloadedFromFront]);
			images[sortedImageIds[firstLoadedFromBack]].unloadFromRam();
		}
		imagesMutex.unlock();
	}
}

void loadNextImage(ImageViewerRenderer& viewer)
{
	if (!viewer.images.empty())
	{
		SharedLockGuard sharedLockGuard(viewer.imagesMutex);
		for (ImageViewerResource& i : viewer.images)
		{
			if (!i.inRam())
			{
				i.loadIntoRam();
				return;
			}
		}
	}
}

void ImageViewerRenderer::calculatePriorities()
{
	for (auto& image : images) image.calculateRating(currentImageId, static_cast<UInt>(images.size()));
}

void ImageViewerRenderer::create(Window& window)
{
	availableMemoryUponStartup = availablePhysicalMemoryInBytes();

	for (const auto& tag : imageTags)
	{
		database.addTag(tag);
	}
}

void ImageViewerRenderer::update(Window& window)
{
	// load new images if dropped
	if (!window.droppedFilePaths.empty())
	{
		// image path to load folder from
		Path imagePath = "";

		// check for new paths
		for (auto p : window.droppedFilePaths)
		{
			// get last path that is actually an image
			if (isImageFile(p)) imagePath = p;
		}

		// clear queue
		window.droppedFilePaths.clear();

		// only if there is a valid image should we reload resources
		if (imagePath != "")
		{
			LockGuard lockGuard(imagesMutex);

			// delete currently loaded resource
			for (auto i : images) i.destroy();
			images.clear();

			// get folder
			Path folderPath = getDirectory(imagePath);

			// get all images in folder
			Vector<Path> paths = getFilesInDirectory(folderPath, imageExtensions);

			// set current index to the originally loaded image
			for (Int i = 0; i < static_cast<Int>(paths.size()); i++)
			{
				//database.add(paths[i]);

				if (paths[i] == imagePath)
				{
					currentImageId = i;
					break;
				}
			}

			// set images to new paths
			for (Index i = 0; i < paths.size(); i++)
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

			for (auto n : sortedIndices) jobSystem.enqueue(loadImage, std::ref(*this), n);
		}
	}

	// input
	if (window.pressed(mouseDown)) offset += window.mouseDelta;

	if (window.pressed(nextImageHolding) || window.pressed(nextImageHoldingMouse)) if (now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showNextImage(window);

	if (window.pressed(previousImageHolding) || window.pressed(previousImageHoldingMouse)) if (now() > lastButtonInput + Milliseconds(static_cast<Int>(1000 * holdingDelay))) showPrevImage(window);
}

void ImageViewer::run(const Int argc, Char* argv[])
{
	ui.create();

	// scan launch parameters for input
	for (Int i = 0; i < argc; i++)
	{
		Path path = argv[i];

		if (isExecutableFile(path)) continue;

		if (isImageFile(path))
		{
			filePaths.push_back(path);
		}
		else
		{
			logWarning("Can't process input: " + path.string());
		}
	}

	Window& window = ui.window("ORKA Image Viewer", Area(1920, 1080), true, true, WindowState::Windowed, renderer);

	for (auto& path : filePaths) window.droppedFilePaths.push_back(path);

	ui.run();
}

void ImageViewerRenderer::connect(GameSimulation& simulation) {}

void ImageViewerRenderer::removeImage(const Int imageId)
{
	if (currentImageId >= imageId) currentImageId--;
	imagesMutex.lock();
	images[imageId].destroy();
	images.erase(images.begin() + imageId);
	imagesMutex.unlock();
}

void ImageViewerRenderer::showNextImage(const Window& window)
{
	const auto currentTime = now();
	if (lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime && !images.empty())
	{
		lastImageRefresh = currentTime;
		currentImageId++;
		if (currentImageId >= static_cast<Int>(images.size())) currentImageId = 0;
		window.setTitle(windowTitleSubstring + images[currentImageId].getName());

		updateLoadedImages();
	}
}

void ImageViewerRenderer::showPrevImage(const Window& window)
{
	if (const auto currentTime = now(); lastImageRefresh + Milliseconds(static_cast<Int>(1000 / frameRate)) < currentTime && !images.empty())
	{
		lastImageRefresh = currentTime;
		currentImageId--;
		if (currentImageId < 0) currentImageId = static_cast<Int>(images.size()) - 1;
		window.setTitle(windowTitleSubstring + images[currentImageId].getName());

		updateLoadedImages();
	}
}

void loadImage(ImageViewerRenderer& viewer, const Index imageId)
{
	if (availablePhysicalMemoryInBytes() > viewer.availableMemoryUponStartup / 3)
	{
		try
		{
			if (viewer.images.size() > imageId)
			{
				viewer.images[imageId].loadIntoRam();
				jobSystem.enqueue(addImageToDatabase, std::ref(viewer), imageId);
			}
		}
		catch (std::exception e)
		{
			logError(e.what());
			viewer.removeImage(imageId);
		}
	}
	else
	{
		logWarning("Image could not be loaded! RAM is full!");
	}
}

void addImageToDatabase(ImageViewerRenderer& viewer, const Index imageId)
{
	viewer.imagesMutex.lock_shared();
	if (viewer.images.size() > imageId)
	{
		const Path   imagePath   = viewer.images[imageId].getPath();
		const String hash        = viewer.images[imageId].getHash();
		const Float  aspectRatio = viewer.images[imageId].getAspectRatio();
		viewer.imagesMutex.unlock_shared();

		viewer.database.add(hash, imagePath);
		viewer.database.setValue(hash, "aspect", aspectRatio);
	}
};

void ImageViewerRenderer::renderInteractive(Window& window, TiledRectangle area) {}

Vector<Index> ImageViewerRenderer::indicesOfImagesSortedByPriority() const
{
	Vector<Index> indices;
	for (Index i = 0; i < images.size(); i++) indices.push_back(i);
	std::ranges::sort(indices, [&](const Index a, const Index b)
	{
		return images[a].getPriority() > images[b].getPriority();
	});
	return indices;
}

void ImageViewerRenderer::render(Window& window, const TiledRectangle area)
{
	Renderer& renderer = window.renderer;

	// images need to be loaded onto gpu in this thread, so lets just do it before we render
	for (Int i = 0; i < static_cast<Int>(images.size()); i++) if (images[i].inRam() && !images[i].onGpu()) images[i].loadOntoGpu();

	renderer.uniforms().reset();

	renderer.clearBackground(Color(0.08, 0.08, 0.08, 1));

	renderer.setAlphaBlending(true, BlendFunction::One, BlendFunction::OneMinusSrcAlpha, BlendEquation::Add);

	renderer.normalizedScreenSpace();

	Float imgX = 0;
	Float imgY = 0;

	if (!images.empty() && images[currentImageId].onGpu())
	{
		images[currentImageId].use(0);
		imgX = static_cast<Float>(images[currentImageId].getWidth());
		imgY = static_cast<Float>(images[currentImageId].getHeight());
	}

	const auto winX = static_cast<Float>(renderer.getWindowWidth());
	const auto winY = static_cast<Float>(renderer.getWindowHeight());

	const Float size = min(winX / imgX, winY / imgY);

	auto finalMatrix = Matrix(1);
	finalMatrix      = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix      = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix      = scale(finalMatrix, Vec3(zoom));

	if (smoothCameraTransition)
	{
		approach(actualMatrix, finalMatrix, renderer.deltaTime() * smoothCameraSpeed);
	}
	else
	{
		actualMatrix = finalMatrix;
	}

	renderer.uniforms().setMMatrix(actualMatrix);

	renderer.useShader("imageViewer");
	renderer.plane();

	//renderer.renderMesh("centeredPlane");

	if (images.size() > 10) renderAllImages(window, area);
}

void ImageViewerRenderer::inputEvent(Window& window, const InputEvent input)
{
	lastButtonInput = now();

	if (input == zoomIn)
	{
		zoomLevel++;
		updateZoom();
	}

	if (input == zoomOut)
	{
		zoomLevel--;
		updateZoom();
	}

	if (input == nextImage || input == nextImageMouse) showNextImage(window);

	if (input == previousImage || input == previousImageMouse) showPrevImage(window);

	if (input == deleteImage)
	{
		if (!images.empty())
		{
			deleteFile(images[currentImageId].getPath());
			removeImage();
		}
	}

	if (input == resetView)
	{
		offset    = Vec2(0);
		zoomLevel = 0;
		updateZoom();
	}
}

void ImageViewerRenderer::renderAllImages(Window& window, const TiledRectangle area)
{
	window.renderer.screenSpace();

	Float xOffset = 0;
	Float yOffset;

	for (Int i = 0; i < static_cast<Int>(images.size()); i++)
	{
		if (ImageViewerResource& image = images[i]; image.onGpu())
		{
			if (currentImageId == i)
			{
				yOffset = desiredHeight / 2;
			}
			else
			{
				yOffset = 0;
			}
			Float       width  = static_cast<Float>(image.getWidth());
			const Float height = static_cast<Float>(image.getHeight());
			width              = width / height * desiredHeight;
			image.use(0);
			window.renderer.uniforms().setMMatrix(matrixFromPositionAndSize(Vec2(xOffset + width / 2, yOffset + desiredHeight / 2), Vec2(width, desiredHeight)));
			window.renderer.plane();
			xOffset += width;
		}
	}
	if (xOffset > 0.0) desiredHeight *= static_cast<Float>(area.size.x) / xOffset;
	desiredHeight = min(desiredHeight, static_cast<Float>(area.size.y) * 0.05f);
}
