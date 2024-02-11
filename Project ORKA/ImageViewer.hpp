#pragma once
#include "FileSystem.hpp"
#include "ResourceManager.hpp"
#include "FileTypes.hpp"
#include "Game.hpp"

// [TODO]
// multithreaded file loading
// usable with only mouse controls
// add fpng support
// add webp support
// converter button

struct ImageViewerResource
{
	String getName();
	void   destroy();
	Path&  getPath();
	void   loadOntoGpu();
	void   loadIntoRam();
	void   unloadFromGpu();
	void   unloadFromRam();
	ImageViewerResource() = delete;
	[[nodiscard]] Bool  inRam() const;
	[[nodiscard]] Bool  onGpu() const;
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] Float getPriority() const;
	void                use(Int textureSlot) const;
	[[nodiscard]] Bool  hasLoadAttemptFailed() const;
	Bool                isBeingLoadedIntoRam = false;
	ImageViewerResource(const Path& path, Index index);
	void calculateRating(Index currentImageIndex, UInt resourceCount);

private:
	CPUTexture cpuTexture;
	GPUTexture gpuTexture;
	Float      priority          = 0;
	Index      index             = 0;
	Bool       loadAttemptFailed = false;
	Path       path              = Path("");
};

struct ImageViewerRenderer final : GameRenderer
{
	Vector<ImageViewerResource> images;
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
	Float                       zoom                       = 1.0f;
	Float                       zoomFactor                 = 1.2f;
	Float                       smoothness                 = 0.1f;
	Float                       holdingDelay               = 0.5f;
	Float                       frameRate                  = 30.0f;
	Float                       smoothCameraSpeed          = 30.0f;
	Bool                        smoothCameraTransition     = false;
	Vec2                        offset                     = Vec2(0);
	Matrix                      actualMatrix               = Matrix(1);
	String                      appName                    = "ORKA Image Viewer";
	String                      windowTitleSubstring       = "ORKA Image Viewer - ";
	// input
	InputId    mouseDown                 = InputId(InputType::Mouse, LMB);
	InputId    previousImageHolding      = InputId(InputType::KeyBoard, LEFT);
	InputId    nextImageHolding          = InputId(InputType::KeyBoard, RIGHT);
	InputEvent zoomIn                    = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOut                   = InputEvent(InputType::Scroll, 1, false);
	InputEvent resetView                 = InputEvent(InputType::Mouse, MMB, false);
	InputId    nextImageHoldingMouse     = InputId(InputType::Mouse, MOUSE_BUTTON_5);
	InputId    previousImageHoldingMouse = InputId(InputType::Mouse, MOUSE_BUTTON_4);
	InputEvent deleteImage               = InputEvent(InputType::KeyBoard, DEL, false);
	InputEvent previousImage             = InputEvent(InputType::KeyBoard, LEFT, false);
	InputEvent nextImage                 = InputEvent(InputType::KeyBoard, RIGHT, false);
	InputEvent nextImageMouse            = InputEvent(InputType::Mouse, MOUSE_BUTTON_5, false);
	InputEvent previousImageMouse        = InputEvent(InputType::Mouse, MOUSE_BUTTON_4, false);

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;

	void updateZoom();
	void updateLoadedImages();
	void calculatePriorities();
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void showNextImage(const Window& window);
	void showPrevImage(const Window& window);
	void connect(GameSimulation& simulation) override;
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderDebugInfo(Window& window, TiledRectangle area) const;
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

void loadImage(ImageViewerRenderer& viewer, Index imageID);
