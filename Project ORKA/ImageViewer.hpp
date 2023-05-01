#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

// [TODO]
// multithreaded file loading
// usable with only mouse controls
// add fpng support
// add webp support
// converter button

//get all resources
//rate them based on some function
//sort them
//increase or decrease the amount of resources loaded based on the memory available
//make sure resources out of that scope are unloaded
//make sure resources in that scope are loaded

struct ImageViewerResource {
	Path path = Path("");
	Float priority = 0;
	Index index = 0;
	CPUTexture cpuTexture;
	GPUTexture gpuTexture;

	void destroy();
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};

struct ImageViewerRenderer : public GameRenderer {
	std::vector<ImageViewerResource> images;
	UInt cpuLoadedCount = 0;
	UInt gpuLoadedCount = 0;
	UInt cpuMaxPreloadCount = 64;
	UInt gpuMaxPreloadCount = 16;

	Int currentImageID = 0;

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

	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, 1);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, 0);
	InputEvent resetView = InputEvent(InputType::Mouse, MMB, 0);
	InputEvent deleteImage = InputEvent(InputType::KeyBoard, DEL, 0);
	InputEvent nextImage = InputEvent(InputType::KeyBoard, RIGHT, 0);
	InputEvent previousImage = InputEvent(InputType::KeyBoard, LEFT, 0);
	InputEvent nextImageMouse = InputEvent(InputType::Mouse, MOUSE_BUTTON_4, 0);
	InputEvent previousImageMouse = InputEvent(InputType::Mouse, MOUSE_BUTTON_5, 0);

	InputID mouseDown = InputID(InputType::Mouse, LMB);
	InputID nextImageHolding = InputID(InputType::KeyBoard, RIGHT);
	InputID previousImageHolding = InputID(InputType::KeyBoard, LEFT);
	InputID nextImageHoldingMouse = InputID(InputType::Mouse, MOUSE_BUTTON_4);
	InputID previousImageHoldingMouse = InputID(InputType::Mouse, MOUSE_BUTTON_5);

	void updateZoom();
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
	Index indexOfCPUImageWithLowestPriority();
	Index indexOfGPUImageWithLowestPriority();
	Index indexOfCPUImageWithHighestPriority();
	Index indexOfGPUImageWithHighestPriority();
	void preloadImage(Path path, GPUTexture& texture);
	std::vector<Index> indicesOfImagesSortedByPriority();
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ImageViewer {
	ResourceManager resourceManager;
	UserInterface ui;
	ImageViewerRenderer renderer;

	void run(Int argc, Char* argv[]);
};