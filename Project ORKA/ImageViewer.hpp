#pragma once
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "Game.hpp"
#include "ImageViewerDatabase.hpp"
#include "ImageViewerResource.hpp"

// [TODO]
// add fpng support
// add animated webp support
// converter button

struct ImageViewerRenderer final : GameRenderer
{
	Vector<ImageViewerResource> images;
	ImageViewerDatabase         database;
	SharedMutex                 imagesMutex;
	TimePoint                   lastButtonInput;
	TimePoint                   lastImageRefresh;
	Int                         zoomLevel                  = 0;
	Float                       holdDelay                  = 1;
	UInt                        currentMemory              = 0;
	UInt                        gpuLoadedCount             = 0;
	UInt                        cpuLoadedCount             = 0;
	Int                         currentImageId             = 0;
	ULong                       availableMemoryUponStartup = 0;
	Float                       desiredHeight              = 100;
	Float                       zoom                       = 1.0f;
	Float                       zoomFactor                 = 1.2f;
	Float                       smoothness                 = 0.1f;
	Float                       holdingDelay               = 0.5f;
	Float                       frameRate                  = 15.0f;
	Float                       smoothCameraSpeed          = 30.0f;
	Bool                        smoothCameraTransition     = false;
	Vec2                        offset                     = Vec2(0);
	Matrix                      actualMatrix               = Matrix(1);
	String                      appName                    = "ORKA Image Viewer";
	String                      windowTitleSubstring       = "ORKA Image Viewer - ";
	// input
	InputId    mouseDown                 = InputId(InputType::Mouse, Lmb);
	InputId    previousImageHolding      = InputId(InputType::KeyBoard, Left);
	InputId    nextImageHolding          = InputId(InputType::KeyBoard, Right);
	InputEvent zoomIn                    = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOut                   = InputEvent(InputType::Scroll, 1, false);
	InputEvent resetView                 = InputEvent(InputType::Mouse, Mmb, false);
	InputId    nextImageHoldingMouse     = InputId(InputType::Mouse, MouseButton5);
	InputId    previousImageHoldingMouse = InputId(InputType::Mouse, MouseButton4);
	InputEvent deleteImage               = InputEvent(InputType::KeyBoard, Del, false);
	InputEvent previousImage             = InputEvent(InputType::KeyBoard, Left, false);
	InputEvent nextImage                 = InputEvent(InputType::KeyBoard, Right, false);
	InputEvent nextImageMouse            = InputEvent(InputType::Mouse, MouseButton5, false);
	InputEvent previousImageMouse        = InputEvent(InputType::Mouse, MouseButton4, false);

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;

	void updateZoom();
	void removeImage();
	void updateLoadedImages();
	void calculatePriorities();
	void removeImage(Int imageId);
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void showNextImage(const Window& window);
	void showPrevImage(const Window& window);
	void connect(GameSimulation& simulation) override;
	void renderAllImages(Window& window, TiledRectangle area);
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
};

struct ImageViewer
{
	UserInterface       ui;
	ImageViewerRenderer renderer;
	Vector<Path>        filePaths;
	Path                currentPath = getCurrentPath();

	void run(Int argc, Char* argv[]);
};

void loadNextImage(ImageViewerRenderer& viewer);

void loadImage(ImageViewerRenderer& viewer, Index imageId);

void addImageToDatabase(ImageViewerRenderer& viewer, Index imageId);
