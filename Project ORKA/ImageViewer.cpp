
#include "ImageViewer.hpp"
#include "FileTypes.hpp"

void ImageViewer::run(Int argc, Char* argv[]) {
	//file management
	Path currentPath = getDirectory(std::filesystem::current_path());
	Vector<Path> filePaths;

	// process parameters
	for (Index i = 0; i < argc; i++) {
		Path path = argv[i];
		if (isExecutableFile(path)) {
			executablePath = getDirectory(path);
			std::filesystem::current_path(executablePath);
		}
		else
		{
			if (isImageFile(path)) {
				filePaths.pushBack(path);
			}
			else {
				logWarning(String("Can't process input: ").append(path.string()));
			}
		}
	}

	ui.create();

	logDebug(executablePath);

	resourceManager.init();

	Window& w = ui.window("ORKA Image Viewer", Area(1920, 1080), true, WindowState::windowed, renderer, resourceManager);

	for (auto& path : filePaths) {
		if (isImageFile(path)) {
			w.droppedFilePaths.pushBack(path);
		}
	}

	ui.run();
}

void ImageViewerRenderer::updateZoom() {
	Float oldZoom = zoom;
	zoom = pow(zoomfactor, zoomLevel);
	offset *= zoom / oldZoom;
}

void ImageViewerRenderer::showNextImage() {
	auto currentTime = now();
	if (lastImageRefresh + Milliseconds(Int(1000 / frameRate)) < currentTime) {
		lastImageRefresh = currentTime;
		currentImageID++;
		if (currentImageID >= images.size()) currentImageID = 0;
	}
}

void ImageViewerRenderer::showPrevImage() {
	auto currentTime = now();
	if (lastImageRefresh + Milliseconds(Int(1000 / frameRate)) < currentTime) {
		lastImageRefresh = currentTime;
		currentImageID--;
		if (currentImageID < 0) currentImageID = images.size() - 1;
	}
}

void ImageViewerRenderer::loadGPUResource() {
	if (images.size()) {
		//if max loaded image count is reached
		if (gpuLoadedCount >= gpuMaxPreloadCount) {
			unloadGPUImageWithLowestPriority();
		}

		//if there are images available to load into ram and we still have space
		if (gpuLoadedCount <= min(gpuMaxPreloadCount, images.size())) {
			loadGPUImageWithHighestPriority();
		}
	}
}

void ImageViewerRenderer::loadCPUResource() {
	if (images.size()) {
		//if max loaded image count is reached
		if (cpuLoadedCount >= cpuMaxPreloadCount) {
			unloadCPUImageWithLowestPriority();
		}

		//if there are images available to load into ram and we still have space
		if (cpuLoadedCount <= min(cpuMaxPreloadCount, images.size())) {
			loadCPUImageWithHighestPriority();
		}
	}
}

void ImageViewerRenderer::calculatePriorities() {
	for (Index i = 0; i < images.size(); i++) {
		images[i].calculateRating(currentImageID, images.size());
	}
}

std::vector<Index> ImageViewerRenderer::indicesOfImagesSortedByPriority() {
	std::vector<Index> indices;
	for (Index i = 0; i < images.size(); i++) {
		indices.push_back(i);
	}
	std::sort(indices.begin(), indices.end(), [&](Index a, Index b) { return images[a].priority > images[b].priority; });
	return indices;
}

void ImageViewerRenderer::update(Window& window) {

	//load new images
	if (window.droppedFilePaths.size()) {

		//image path to load folder from
		Path imagePath = "";

		//check for new paths
		for (auto p : window.droppedFilePaths) {
			//get last path that is actually an image
			if (isImageFile(p)) imagePath = p;
		}

		//clear queue
		window.droppedFilePaths.clear();

		//only if there is a valid image should we reload resources
		if (imagePath != "") {
			//delete currently loaded resource
			for (auto i : images) i.destroy();
			images.clear();

			//get folder
			Path folderPath = getDirectory(imagePath);

			//get all images in folder
			Vector<Path> paths = getFilesInDirectory(folderPath, imageExtensions);

			//set current index to the originally loaded image
			for (Index i = 0; i < paths.size(); i++) {
				if (paths[i] == imagePath) {
					currentImageID = i;
					break;
				}
			}

			//set images to new paths
			for (Index i = 0; i < paths.size(); i++) {
				ImageViewerResource image;
				image.path = paths[i];
				image.index = i;
				images.push_back(image);
			}
		}
	}

	//input
	if (window.pressed(mouseDown)) {
		offset += window.mouseDelta;
	}

	//load resources
	calculatePriorities();

	//get vector of indices sorted by priority
	if (images.size()) {
		std::vector<Index> sortedIndices = indicesOfImagesSortedByPriority();

		//make sure were not loading more than we can
		if(sortedIndices.size() > cpuMaxPreloadCount)sortedIndices.resize(cpuMaxPreloadCount);

		for (UInt order = 0; order < sortedIndices.size(); order++) {
			Index currentImageIndex = sortedIndices[order];

			if (order < cpuMaxPreloadCount) {
				images[currentImageIndex].cpuTexture.load(images[currentImageIndex].path, "imageViewerImage");

				if (order < gpuMaxPreloadCount) {
					if(!images[currentImageIndex].gpuTexture.loaded) images[currentImageIndex].gpuTexture.load(images[currentImageIndex].cpuTexture);
				}
				else {
					images[currentImageIndex].gpuTexture.unload();
				}
			}
			else {
				images[currentImageIndex].cpuTexture.unload();
				images[currentImageIndex].gpuTexture.unload();
			}
		}
	}

	
	if (window.pressed(nextImageHolding) || window.pressed(nextImageHoldingMouse)) {
		if (now() > lastButtonInput + Milliseconds(Int(1000 * holdingDelay)))
			showNextImage();
	}

	if (window.pressed(previousImageHolding) || window.pressed(previousImageHoldingMouse)) {
		if (now() > lastButtonInput + Milliseconds(Int(1000 * holdingDelay)))
			showPrevImage();
	}
}

void ImageViewerRenderer::loadGPUImageWithHighestPriority() {
	if (images.size()) {
		Index imageID = indexOfGPUImageWithHighestPriority();
		images[imageID].gpuTexture.load(images[imageID].cpuTexture);
		if (images[imageID].gpuTexture.loaded)gpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::loadCPUImageWithHighestPriority() {
	if (images.size()) {
		Index imageID = indexOfCPUImageWithHighestPriority();
		images[imageID].cpuTexture.load(images[imageID].path, "imageViewerImage");
		if (images[imageID].cpuTexture.loaded)cpuLoadedCount++;
		else logError("Image could not be loaded!");
	}
}

void ImageViewerRenderer::unloadGPUImageWithLowestPriority() {
	if (images.size()) {
		Index imageID = indexOfGPUImageWithLowestPriority();
		images[imageID].gpuTexture.unload();
		if (!images[imageID].gpuTexture.loaded)gpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

void ImageViewerRenderer::unloadCPUImageWithLowestPriority() {
	if (images.size()) {
		Index imageID = indexOfCPUImageWithLowestPriority();
		images[imageID].cpuTexture.unload();
		if (!images[imageID].cpuTexture.loaded)cpuLoadedCount--;
		else logError("Image could not be unloaded!");
	}
}

Index ImageViewerRenderer::indexOfGPUImageWithLowestPriority() {
	if (!images.size())logError("No images to get index from!");

	Float lowestPriority = 0;
	Index lowestPriorityIndex = 0;
	for (Int i = 0; i < images.size(); i++) {
		if (images[i].gpuTexture.loaded) {
			lowestPriority = images[i].priority;
			lowestPriorityIndex = i;
		}
	}
	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCPUImageWithLowestPriority() {
	if (!images.size())logError("No images to get index from!");

	Float lowestPriority = 0;
	Index lowestPriorityIndex = 0;

	for (Int i = 0; i < images.size(); i++) {
		if (images[i].cpuTexture.loaded && images[i].gpuTexture.loaded != true) {
			lowestPriority = images[i].priority;
			lowestPriorityIndex = i;
		}
	}

	return lowestPriorityIndex;
}

Index ImageViewerRenderer::indexOfCPUImageWithHighestPriority() {
	if (!images.size())logError("No images to get index from!");

	Float highestPriority = 0;
	Index highestPriorityIndex = 0;

	//find the first image that is not yet loaded
	for (Int i = 0; i < images.size(); i++) {
		if (images[i].cpuTexture.loaded != true) {
			highestPriority = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for (Index i = highestPriorityIndex; i < images.size(); i++) {
		if (images[i].cpuTexture.loaded != true) {
			if (images[i].priority > highestPriority) {
				highestPriority = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

Index ImageViewerRenderer::indexOfGPUImageWithHighestPriority() {
	if (!images.size())logError("No images to get index from!");

	Float highestPriority = 0;
	Index highestPriorityIndex = 0;
	//find the first image that is not yet loaded
	for (Int i = 0; i < images.size(); i++) {
		if (images[i].cpuTexture.loaded && !images[i].gpuTexture.loaded) {
			highestPriority = images[i].priority;
			highestPriorityIndex = i;
			break;
		}
	}

	//get the image with the highest priority
	for (Index i = highestPriorityIndex; i < images.size(); i++) {
		if (images[i].cpuTexture.loaded && !images[i].gpuTexture.loaded) {
			if (images[i].priority > highestPriority) {
				highestPriority = images[i].priority;
				highestPriorityIndex = i;
			}
		}
	}

	return highestPriorityIndex;
}

void ImageViewerRenderer::preloadImage(Path path, GPUTexture& texture) {
	if (texture.loaded) texture.unload();
	CPUTexture cpuTexture;
	cpuTexture.unload();
	cpuTexture.nearFilter = Filter::nearest;
	cpuTexture.load(path, "imageViewerImage");
	texture.load(cpuTexture);
};

void ImageViewerRenderer::inputEvent(Window& window, InputEvent input) {

	lastButtonInput = now();

	if (input == zoomIn) {
		zoomLevel++;
		updateZoom();
	}

	if (input == zoomOut) {
		zoomLevel--;
		updateZoom();
	}

	if (input == nextImage || input == nextImageMouse) {
		showNextImage();
	}

	if (input == previousImage || input == previousImageMouse) {
		showPrevImage();
	}

	if (input == deleteImage) {
		if (images.size()) {
			deleteFile(images[currentImageID].path);
			images[currentImageID].destroy();
			images.erase(images.begin() + currentImageID);
			if (currentImageID >= images.size())currentImageID--;
		}
	}

	if (input == resetView) {
		offset = Vec2(0);
		zoomLevel = 0;
		updateZoom();
	}
}

void ImageViewerRenderer::render(ResourceManager& resourceManager, Window& window, TiledRectangle area) {

	Renderer& renderer = window.renderer;

	renderer.uniforms().reset();

	renderer.draw("main");

	renderer.clearColor(Color(0.08, 0.08, 0.08, 1));

	renderer.setAlphaBlending(true, BlendFunction::one, BlendFunction::oneMinusSrcAlpha, BlendEquation::add);

	renderer.normalizedScreenSpace();

	Float imgX = 0;
	Float imgY = 0;

	if (images.size()) {
		images[currentImageID].gpuTexture.use(0);
		imgX = images[currentImageID].cpuTexture.width;
		imgY = images[currentImageID].cpuTexture.height;
	}

	Float winX = renderer.renderRegion.region.size.x;
	Float winY = renderer.renderRegion.region.size.y;

	Float size = min(winX / imgX, winY / imgY);

	Matrix finalMatrix = Matrix(1);
	finalMatrix = translate(finalMatrix, Vec3(offset, 0));
	finalMatrix = scale(finalMatrix, Vec3(size * imgX, size * imgY, 0));
	finalMatrix = scale(finalMatrix, Vec3(zoom));

	approach(actualMatrix, finalMatrix, renderer.deltaTime() * 10);

	renderer.uniforms().mMatrix() = actualMatrix;

	renderer.useShader(resourceManager, "imageViewer");
	renderer.renderMesh(resourceManager, "centeredPlane");
}

void ImageViewerResource::destroy() {
	cpuTexture.unload();
	gpuTexture.unload();
}

void ImageViewerResource::calculateRating(Index currentImageIndex, UInt resourceCount) {
	Float b = resourceCount;
	Float a = currentImageIndex;
	Float x = index;
	const Float diff_a_x = abs(a - x);
	priority = b / 2.0 - min(abs(a - x - b), min(diff_a_x, abs(a - x + b)));
}
