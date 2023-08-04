#pragma once
#include "Game.hpp"
#include "JobSystem.hpp"
#include "ResourceManager.hpp"
// [TODO]
// multithreaded file loading
// usable with only mouse controls
// add fpng support
// add webp support
// converter button

struct ImageViewerResource
{
	void  destroy();
	Path& getPath();
	void  loadOntoGpu();
	void  loadIntoRam();
	void  unloadFromGpu();
	void  unloadFromRam();
	ImageViewerResource() = delete;
	void                use(Int textureSlot) const;
	[[nodiscard]] Bool  inRam() const;
	[[nodiscard]] Bool  onGpu() const;
	[[nodiscard]] Int   getWidth() const;
	[[nodiscard]] Int   getHeight() const;
	[[nodiscard]] Float getPriority() const;
	[[nodiscard]] Bool  hasLoadAttemptFailed() const;
	ImageViewerResource(const Path& path, Index index);
	void calculateRating(Index currentImageIndex, UInt resourceCount);
private:
	Bool       loadAttemptFailed = false;
	Path       path              = Path("");
	Float      priority          = 0;
	Index      index             = 0;
	CPUTexture cpuTexture;
	GPUTexture gpuTexture;
};

struct ImageViewerRenderer final : GameRenderer
{
	// JobSystem resourceLoaderThreads;
	Vector<ImageViewerResource> images;
	TimePoint                   lastButtonInput;
	TimePoint                   lastImageRefresh;
	Float                       holdDelay          = 1;
	UInt                        cpuLoadedCount     = 0;
	UInt                        gpuLoadedCount     = 0;
	UInt                        cpuMaxPreLoadCount = 2;
	UInt                        gpuMaxPreLoadCount = 1;
	Int                         currentImageId     = 0;
	Int                         zoomLevel          = 0;
	Float                       zoom               = 1.0f;
	Float                       holdingDelay       = 0.5f;
	Float                       zoomFactor         = 1.2f;
	Float                       smoothness         = 0.1f;
	Float                       frameRate          = 15.0f;
	Vec2                        offset             = Vec2(0);
	Matrix                      actualMatrix       = Matrix(1);
	// input
	InputId    mouseDown                 = InputId(InputType::Mouse, LMB);
	InputId    previousImageHolding      = InputId(InputType::KeyBoard, LEFT);
	InputId    nextImageHolding          = InputId(InputType::KeyBoard, RIGHT);
	InputEvent zoomIn                    = InputEvent(InputType::Scroll, 1, true);
	InputEvent zoomOut                   = InputEvent(InputType::Scroll, 1, false);
	InputEvent resetView                 = InputEvent(InputType::Mouse, MMB, false);
	InputId    nextImageHoldingMouse     = InputId(InputType::Mouse, MOUSE_BUTTON_4);
	InputId    previousImageHoldingMouse = InputId(InputType::Mouse, MOUSE_BUTTON_5);
	InputEvent deleteImage               = InputEvent(InputType::KeyBoard, DEL, false);
	InputEvent previousImage             = InputEvent(InputType::KeyBoard, LEFT, false);
	InputEvent nextImage                 = InputEvent(InputType::KeyBoard, RIGHT, false);
	InputEvent nextImageMouse            = InputEvent(InputType::Mouse, MOUSE_BUTTON_4, false);
	InputEvent previousImageMouse        = InputEvent(InputType::Mouse, MOUSE_BUTTON_5, false);

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithHighestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithHighestPriority() const;

	void updateZoom();
	void removeImage();
	void preLoadImages();
	void showNextImage();
	void showPrevImage();
	void loadGpuResource();
	void loadCpuResource();
	void calculatePriorities();
	void update(Window& window) override;
	void destroy(Window& window) override;
	void loadCpuImageWithHighestPriority();
	void loadGpuImageWithHighestPriority();
	void unloadGpuImageWithLowestPriority();
	void unloadCpuImageWithLowestPriority();
	void inputEvent(Window& window, InputEvent input) override;
	void create(ResourceManager& resourceManager, Window& window) override;
	void render(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
	void renderInteractive(ResourceManager& resourceManager, Window& window, TiledRectangle area) override;
};

struct ImageViewer
{
	UserInterface       ui;
	ImageViewerRenderer renderer;
	ResourceManager     resourceManager;
	void                run(Int argc, Char* argv[]);
};
