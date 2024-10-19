#pragma once

#include "FileSystem.hpp"
#include "FileTypes.hpp"
#include "Game.hpp"

struct SortrResource
{
	CpuTexture cpuTexture;
	GPUTexture gpuTexture;
	Index      index    = 0;
	Float      priority = 0;
	Path       path     = Path("");

	void destroy(Renderer& renderer);
	void calculateRating(Index currentImageIndex, UInt resourceCount);
};

struct SortrFolderElement
{
	Path       path = "";
	Float      size = 0.0;
	String     folderName;
	Vec2       position = Vec2(0, 0);
	InputEvent keyBind  = InputEvent(InputType::Unknown, Unknown, true);
};

struct SortrFolderRing
{
	Vector<SortrFolderElement> elements;
	Bool                       shapeToggle = false;

	void add();
	void create();
	void updatePosition();
	void render(Renderer& renderer) const;
};

struct SortrRenderer final : GameRenderer
{
	SortrFolderRing       ring;
	Vector<SortrResource> images;
	TimePoint             lastButtonInput;
	TimePoint             lastImageRefresh;
	UInt                  cpuLoadedCount     = 0;
	UInt                  gpuLoadedCount     = 0;
	UInt                  cpuMaxPreloadCount = 2;
	UInt                  gpuMaxPreloadCount = 1;
	Int                   currentImageId     = 0;
	Int                   zoomLevel          = 0;
	Float                 holdDelay          = 1.0f;
	Float                 holdingDelay       = 0.5f;
	Float                 zoom               = 1.0f;
	Float                 zoomFactor         = 1.2f;
	Float                 smoothness         = 0.1f;
	Float                 frameRate          = 60.0f;
	Vec2                  offset             = Vec2(0.0f);
	Matrix                actualMatrix       = Matrix(1.0f);
	InputId               mouseDown          = InputId(InputType::Mouse, Lmb);
	InputEvent            zoomIn             = InputEvent(InputType::Scroll, 1, true);
	InputEvent            zoomOut            = InputEvent(InputType::Scroll, 1, false);
	InputEvent            resetView          = InputEvent(InputType::Mouse, Mmb, false);
	InputEvent            click              = InputEvent(InputType::Mouse, Lmb, false);
	InputEvent            shapeToggler       = InputEvent(InputType::KeyBoard, O, true);
	InputEvent            deleteImage        = InputEvent(InputType::KeyBoard, Del, false);
	InputEvent            left               = InputEvent(InputType::KeyBoard, Left, false);
	InputEvent            right              = InputEvent(InputType::KeyBoard, Right, false);

	void updateZoom();
	void showNextImage();
	void showPrevImage();
	void sortLeft() const;
	void loadCpuResource();
	void sortRight() const;
	void calculatePriorities();
	void addSortrFolderElement();
	void update(Window& window) override;
	void create(Window& window) override;
	void destroy(Window& window) override;
	void preloadImages(Renderer& renderer);
	void loadCpuImageWithHighestPriority();
	void unloadCpuImageWithLowestPriority();
	void loadGpuResource(Renderer& renderer);
	void connect(GameSimulation& simulation) override;
	void loadGpuImageWithHighestPriority(Renderer& renderer);
	void unloadGpuImageWithLowestPriority(Renderer& renderer);
	void render(Window& window, TiledRectangle area) override;
	void inputEvent(Window& window, InputEvent input) override;
	void renderInteractive(Window& window, TiledRectangle area) override;
	void preloadImage(Renderer& renderer, const Path& path, GPUTexture& texture) const;

	[[nodiscard]] Vector<Index> indicesOfImagesSortedByPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithLowestPriority() const;
	[[nodiscard]] Index         indexOfCpuImageWithHighestPriority() const;
	[[nodiscard]] Index         indexOfGpuImageWithHighestPriority() const;
};

struct Sortr
{
	SortrRenderer renderer;
	Vector<Path>  filePaths;
	Path          currentPath = getCurrentPath();

	void run(const Int argc, Char* argv[]);
};
