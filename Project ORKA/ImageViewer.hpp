#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

// [TODO]
// multithreaded file loading
// usable with only mouse controls
// add fpng support
// add webp support
// converter button

struct ImageViewerResource {
	Path path = Path("");
	Float priority = 0;
	Index index = 0;
	CPUTexture cpuTexture;
	GpuTexture gpuTexture;

	void destroy();
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};

struct ImageViewerRenderer : public GameRenderer {
	Vector<ImageViewerResource> images;
	UInt cpuLoadedCount = 0;
	UInt gpuLoadedCount = 0;
	UInt cpuMaxPreloadCount = 2;
	UInt gpuMaxPreloadCount = 1;

	Int currentImageId = 0;

	Float frameRate = 15;
	Float holdingDelay = 0.5;

	Int zoomLevel = 0;
	Float zoom = 1;
	Float zoomfactor = 1.2;
	Float smoothness = 0.1;

	TimePoint lastImageRefresh;
	TimePoint lastButtonInput;

	Vec2 offset = Vec2(0);

	Matrix actualMatrix = Matrix(1);

	Float holdDelay = 1;

	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, false);
	InputEvent resetView = InputEvent(InputType::Mouse, MMB, false);
	InputEvent deleteImage = InputEvent(InputType::KeyBoard, DEL, false);
	InputEvent nextImage = InputEvent(InputType::KeyBoard, RIGHT, false);
	InputEvent previousImage = InputEvent(InputType::KeyBoard, LEFT, false);
	InputEvent nextImageMouse = InputEvent(InputType::Mouse, MOUSE_BUTTON_4, false);
	InputEvent previousImageMouse = InputEvent(InputType::Mouse, MOUSE_BUTTON_5, false);

	InputId mouseDown = InputId(InputType::Mouse, LMB);
	InputId nextImageHolding = InputId(InputType::KeyBoard, RIGHT);
	InputId previousImageHolding = InputId(InputType::KeyBoard, LEFT);
	InputId nextImageHoldingMouse = InputId(InputType::Mouse, MOUSE_BUTTON_4);
	InputId previousImageHoldingMouse = InputId(InputType::Mouse, MOUSE_BUTTON_5);

	void updateZoom();
	void preloadImages();
	void showNextImage();
	void showPrevImage();
	void loadGPUResource();
	void loadCPUResource();
	void calculatePriorities();
	void update(Window& window) override;
	void loadCPUImageWithHighestPriority();
	void loadGPUImageWithHighestPriority();
	void unloadGPUImageWithLowestPriority();
	void unloadCPUImageWithLowestPriority();
	Index indexOfCPUImageWithLowestPriority() const;
	Index indexOfGPUImageWithLowestPriority() const;
	Index indexOfCPUImageWithHighestPriority() const;
	Index indexOfGPUImageWithHighestPriority() const;
	void preloadImage(const Path& path, GpuTexture& texture);
	Vector<Index> indicesOfImagesSortedByPriority() const;
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ImageViewer {
	ResourceManager resourceManager;
	UserInterface ui;
	ImageViewerRenderer renderer;

	ImageViewer(Int argc, Char* argv[]);
};
