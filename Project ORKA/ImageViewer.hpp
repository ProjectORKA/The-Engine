#pragma once

#include "Game.hpp"
#include "ResourceManager.hpp"

// [TODO]
// multithreaded file loading
// only load files in folder
// usable with only mouse controls
// add fpng support
// add webp support
// converter button

struct ImageViewerRenderer : public GameRenderer {
	Vector<Path> paths;
	Vector<GPUTexture> gpuTextures;

	Vector<Thread> threads;
	Float frameRate = 24;
	Float holdingDelay = 0.5;

	Int currentImageID = 0;

	Int zoomLevel = 0;
	Float zoom = 1;
	Float zoomfactor = 1.2;
	Float smoothness = 0.1;
	
	TimePoint lastImageRefresh;
	TimePoint lastButtonInput;

	Vec2 offset = Vec2(0);

	Matrix actualMatrix = Matrix(1);

	Float holdDelay = 1;

	InputEvent previousImage = InputEvent(InputType::KeyBoard, LEFT, 0);
	InputEvent deleteImage = InputEvent(InputType::KeyBoard, DEL, 0);
	InputEvent nextImage = InputEvent(InputType::KeyBoard, RIGHT, 0);
	InputEvent zoomIn = InputEvent(InputType::Scroll, 1, 1);
	InputEvent zoomOut = InputEvent(InputType::Scroll, 1, 0);
	InputEvent resetView = InputEvent(InputType::Mouse, MMB, 0);

	InputID mouseDown = InputID(InputType::Mouse, LMB);
	InputID nextImageHolding = InputID(InputType::KeyBoard, RIGHT);
	InputID previousImageHolding = InputID(InputType::KeyBoard, LEFT);

	void updateZoom();
	void setNextImage();
	void setPreviousImage();
	void update(Window& window) override;
	void preloadImage(Path path, GPUTexture& texture);
	void inputEvent(Window& window, InputEvent input) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ImageViewer {
	ResourceManager resourceManager;
	UserInterface ui;
	ImageViewerRenderer renderer;

	void run(Int argc, Char* argv[]);
};