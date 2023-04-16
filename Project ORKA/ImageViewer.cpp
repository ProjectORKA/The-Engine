
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

void ImageViewerRenderer::setNextImage() {
	auto currentTime = now();
	if (lastImageRefresh + Milliseconds(Int(1000 / frameRate)) < currentTime) {
		lastImageRefresh = currentTime;
		currentImageID++;
		if (currentImageID >= paths.size()) currentImageID = 0;
	}
}
void ImageViewerRenderer::updateZoom() {
	Float oldZoom = zoom;
	zoom = pow(zoomfactor, zoomLevel);
	offset *= zoom / oldZoom;
}
void ImageViewerRenderer::setPreviousImage() {
	auto currentTime = now();
	if (lastImageRefresh + Milliseconds(Int(1000 / frameRate)) < currentTime) {
		lastImageRefresh = currentTime;
		currentImageID--;
		if (currentImageID < 0) currentImageID = paths.size() - 1;
	}
}
void ImageViewerRenderer::update(Window& window) {
	if (window.droppedFilePaths.size()) {
		Path imagePath = window.droppedFilePaths.last();
		Path folderPath = getDirectory(imagePath);
		if (isImageFile(imagePath)) {
			paths = getFilesInDirectory(folderPath, imageExtensions);

			//get all relevant paths
			for (Index i = 0; i < paths.size(); i++) {
				if (paths[i] == imagePath) currentImageID = i;
			}

			//unload gpuTextures
			for (Index i = 0; i < gpuTextures.size(); i++) gpuTextures[i].unload();
			gpuTextures.resize(paths.size());

			//load all gpu textures
			for (Index i = 0; i < paths.size(); i++) {
				preloadImage(paths[i], gpuTextures[i]);
			}

			window.droppedFilePaths.popBack();
		}
		else logWarning("File not an Image!");
	}

	if (window.pressed(mouseDown)) {
		offset += window.mouseDelta;
	}

	if (window.pressed(nextImageHolding)) {
		if (now() > lastButtonInput + Milliseconds(Int(1000 * holdingDelay)))
			setNextImage();
	}

	if (window.pressed(previousImageHolding)) {
		if (now() > lastButtonInput + Milliseconds(Int(1000 * holdingDelay)))
			setPreviousImage();
	}
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

	if (input == nextImage) {
		setNextImage();
	}

	if (input == previousImage) {
		setPreviousImage();
	}

	if (input == deleteImage) {
		if (paths.size()) {
			deleteFile(paths[currentImageID]);
			paths.erase(currentImageID);
			gpuTextures.erase(currentImageID);
			if (currentImageID >= paths.size())currentImageID--;
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

	if (gpuTextures.size()) {
		gpuTextures[currentImageID].use(0);
		imgX = gpuTextures[currentImageID].width;
		imgY = gpuTextures[currentImageID].height;
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
